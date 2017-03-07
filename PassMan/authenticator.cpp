/*
 * Description: Implementation of the Authenticator class.
 *              Handles secure file encryption/decryption operations.
 *              Utilizes AES-256 in GCM-AE mode.
 *              Two factors are used for the key: A user password, and their YubiKey's HMAC-SHA1 response.
 *              They are combined to a single master key via PBKDF2-SHA512.
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#include "authenticator.h"
#include "ui_authenticator.h"

const char Authenticator::FILE_PORTION_SEPARATOR = ':';
const int Authenticator::TAG_SIZE = 16;
const double Authenticator::MIN_PBKDF_TIME = 0.5;
const int Authenticator::DECRYPT_MODE = 0;
const int Authenticator::ENCRYPT_MODE = 1;
const QString Authenticator::WAITING = "Waiting for key";
const QString Authenticator::BUSY_YUBIKEY = "Contacting YubiKey";
const QString Authenticator::BUSY_KEY = "Computing key";
const QString Authenticator::FAILED = "Failed";
const QString Authenticator::COMPLETE = "Valid key";
const QString Authenticator::ERROR_TITLE = "Authenticator Error";
const QString Authenticator::DB_ERROR = "Unable to open database.";
const QString Authenticator::YUBIKEY_ERROR = "Unable to challenge YubiKey.";
const QString Authenticator::YUBIKEY_HMAC_ERROR = "The wrong configuration slot may be selected.";
const QString Authenticator::YUBIKEY_PRESENT_ERROR = "The YubiKey may not be connected.";
const QString Authenticator::DECRYPT_ERROR = "Unable to decrypt the database.";
const QString Authenticator::ENCRYPT_ERROR = "Unable to encrypt the database.";
const QString Authenticator::FILE_ERROR = "The file could not be opened for reading.";
const QString Authenticator::PIECES_ERROR = "The file is missing required pieces.";
const QString Authenticator::HMAC_ERROR = "The YubiKey HMAC challenge is invalid.";
const QString Authenticator::IV_ERROR = "The initialization vector is invalid.";
const QString Authenticator::CIPHER_ERROR = "The ciphertext is invalid.";
const QString Authenticator::SALT_ERROR = "The salt is invalid.";
const QString Authenticator::INTEGRITY_ERROR = "The key is incorrect, or the database file is corrupted.";
const QString Authenticator::ITERATION_ERROR = " The iteration count is invalid.";

Authenticator::Authenticator(YubiKey* yk, QWidget *parent) : QMainWindow(parent), ui(new Ui::Authenticator)
{
    ui->setupUi(this);
    yubikey = yk;
    operationMode = DECRYPT_MODE;
    connect(yubikey, SIGNAL(yubiKeyChanged()), this, SLOT(updateYubiKeyState()));    // Update details if YubiKey plugged in
    setStatus(WAITING);
    yubikeyState = new QLabel();
    statusBar()->addPermanentWidget(yubikeyState);
    statusBar()->addPermanentWidget(new QLabel(" "));   // Dummy label to add space on right of statusBar
    yubikey->poll();
    updateYubiKeyState();
}

Authenticator::~Authenticator()
{
    clean();    // Wipe sensitive variables prior to deconstruction!
    delete ui;
}

void Authenticator::open(const QString& fileName, Database* db) // Decrypt a file
{
    ui->masterPasswordLineEdit->clear();
    operationMode = DECRYPT_MODE;
    this->fileName = fileName;
    this->db = db;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))    // Failed to open file
    {
        notify(QMessageBox::Critical, ERROR_TITLE, DB_ERROR, FILE_ERROR);
        this->clean();
        this->hide();
        return;
    }
    QByteArrayList parts = file.readAll().split(FILE_PORTION_SEPARATOR);
    file.close();
    if (parts.length() != 5)    // File is missing crucial parts
    {
        notify(QMessageBox::Critical, ERROR_TITLE, DB_ERROR, PIECES_ERROR);
        this->clean();
        this->hide();
        return;
    }
    challenge = QByteArray::fromBase64(parts.at(0));    // Store the challenge, iv, and cipher from the file
    if (challenge.length() != YubiKey::MAX_HMAC_CHALLENGE_SIZE) // Challenge portion is invalid
    {
        notify(QMessageBox::Critical, ERROR_TITLE, DB_ERROR, HMAC_ERROR);
        this->clean();
        this->hide();
        return;
    }
    QByteArray salt = QByteArray::fromBase64(parts.at(1));
    if (salt.length() != SALT_SIZE) // Salt portion is invalid
    {
        notify(QMessageBox::Critical, ERROR_TITLE, DB_ERROR, SALT_ERROR);
        this->clean();
        this->hide();
        return;
    }
    for (int i = 0; i < SALT_SIZE; i++) this->salt[i] = salt.at(i);
    QByteArray iters = QByteArray::fromBase64(parts.at(2));
    iterations = iters.toInt();
    if (iterations < 1)    // Iterations portion is invalid
    {
        notify(QMessageBox::Critical, ERROR_TITLE, DB_ERROR, ITERATION_ERROR);
        this->clean();
        this->hide();
        return;
    }
    QByteArray iv = QByteArray::fromBase64(parts.at(3));
    if (iv.length() != IV_SIZE) // Initialization vector is invalid
    {
        notify(QMessageBox::Critical, ERROR_TITLE, DB_ERROR, IV_ERROR);
        this->clean();
        this->hide();
        return;
    }
    for (int i = 0; i < IV_SIZE; i++) this->iv[i] = iv.at(i);
    cipher = QByteArray::fromBase64(parts.at(4)).toStdString();
    if (cipher.length() < 1)    // Ciphertext is invalid
    {
        notify(QMessageBox::Critical, ERROR_TITLE, DB_ERROR, CIPHER_ERROR);
        this->clean();
        this->hide();
        return;
    }
    this->show();   // Show interface to user
}

void Authenticator::save(const QString& fileName, Database* db) // Encrypt a file
{
    try
    {
        operationMode = ENCRYPT_MODE;
        this->fileName = fileName;
        this->db = db;
        byte challenge[YubiKey::MAX_HMAC_CHALLENGE_SIZE];
        CryptoPP::AutoSeededRandomPool prng;
        prng.GenerateBlock(challenge, sizeof(challenge));   // Generate new random HMAC challenge each time!
        this->challenge.clear();
        for (int i = 0; i < YubiKey::MAX_HMAC_CHALLENGE_SIZE; i++) this->challenge.append(challenge[i]);
        prng.GenerateBlock(iv, sizeof(iv)); // Generate new random IV each time!
        prng.GenerateBlock(salt, sizeof(salt)); // Generate new random salt each time!
    }
    catch (CryptoPP::Exception& ex) //Catch if challenge and iv generation fail
    {
        setStatus(FAILED);
        notify(QMessageBox::Critical, ERROR_TITLE, ENCRYPT_ERROR, QString(ex.what()));
        this->clean();
        this->hide();
        return;
    }
    QJsonObject obj;
    db->write(obj);
    QJsonDocument doc(obj);
    clear = doc.toJson().toStdString();
    this->show();   // Continue process after user supplies password
}

void Authenticator::formKey()   // Create master key
{
    if (canChallenge)
    {
        setStatus(BUSY_YUBIKEY);
        response = yubikey->hmacSHA1(challenge, true);
        yubikeyState->setText(yubikey->stateText());
        if (yubikey->state() == YubiKey::NOT_PRESENT)
        {
            notify(QMessageBox::Warning, ERROR_TITLE, YUBIKEY_ERROR, YUBIKEY_PRESENT_ERROR);
            return;
        }
        else if (yubikey->state() == YubiKey::TIMEOUT)
        {
            notify(QMessageBox::Warning, ERROR_TITLE, YUBIKEY_ERROR, YUBIKEY_HMAC_ERROR);
            return;
        }
        response.append(ui->masterPasswordLineEdit->text());
        setStatus(BUSY_KEY);
        CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA512> kdf;  // Derive master key from concatenation of user password and YubiKey response
        if (operationMode == DECRYPT_MODE)
        {
            kdf.DeriveKey(this->key, sizeof(key), 0, (byte*) response.data(), response.length(), this->salt, sizeof(salt), iterations, 0);  // Use recovered iteration count to derive key
            if (!decrypt()) return;
            db->read(QJsonDocument::fromJson(QByteArray::fromStdString(clear)).object());
        }
        else
        {
            iterations =  kdf.DeriveKey(this->key, sizeof(key), 0, (byte*) response.data(), response.length(), this->salt, sizeof(salt), iterations, MIN_PBKDF_TIME);
            if (!encrypt()) return;
            QFile file(fileName);
            QByteArray iv;
            for (int i = 0; i < IV_SIZE; i++) iv.append(this->iv[i]);
            QByteArray salt;
            for (int i = 0; i < SALT_SIZE; i++) salt.append(this->salt[i]);
            file.open(QIODevice::WriteOnly);
            file.write(challenge.toBase64());
            file.write(":");
            file.write(salt.toBase64());
            file.write(":");
            file.write(QByteArray::number(iterations).toBase64());
            file.write(":");
            file.write(iv.toBase64());
            file.write(":");
            file.write(QByteArray::fromStdString(cipher).toBase64());
            file.close();
        }
        this->hide();
    }
}

void Authenticator::clean() // Reset authenticator and wipe any sensitive data
{
    for (int i = 0; i < CryptoPP::AES::MAX_KEYLENGTH; i++) key[i] = 0;
    for (int i = 0; i < IV_SIZE; i++) iv[i] = 0;
    for (int i = 0; i < SALT_SIZE; i++) salt[i] = 0;
    challenge.fill(0);
    response.fill(0);
    clear.assign(clear.length(), 0);
    cipher.assign(cipher.length(), 0);
}

int Authenticator::encrypt()    // Perform authenticated AES-256 encryption in GCM-AE mode
{
    try
    {
        cipher.clear();
        CryptoPP::GCM<CryptoPP::AES>::Encryption enc;
        enc.SetKeyWithIV(key, sizeof(key), iv, sizeof(iv)); // Initialize cipher
        CryptoPP::StringSource src(clear, true, new CryptoPP::AuthenticatedEncryptionFilter(enc, new CryptoPP::StringSink(cipher), false, TAG_SIZE));   // Run cleartext through
    }
    catch (CryptoPP::Exception& ex)
    {
        setStatus(FAILED);
        notify(QMessageBox::Critical, ERROR_TITLE, ENCRYPT_ERROR, QString(ex.what()));
        this->clean();
        this->hide();
        return false;
    }
    setStatus(COMPLETE);
    return true;
}

int Authenticator::decrypt()    // Perform authenticated AES-256 decryption in GCM-AE mode
{
    try
    {
        clear.clear();
        CryptoPP::GCM<CryptoPP::AES>::Decryption dec;
        dec.SetKeyWithIV(key, sizeof(key), iv, sizeof(iv)); // Initialize cipher
        CryptoPP::AuthenticatedDecryptionFilter adf(dec, new CryptoPP::StringSink(clear), CryptoPP::AuthenticatedDecryptionFilter::DEFAULT_FLAGS, TAG_SIZE);    // Initialize authentication filter
        CryptoPP::StringSource src(cipher, true, new CryptoPP::Redirector(adf));    // Redirector feeds cipher into authenticator
    }
    catch (CryptoPP::Exception& ex) // Will catch if integrity check fails, or other issue
    {
        setStatus(FAILED);
        if (ex.GetErrorType() == CryptoPP::Exception::DATA_INTEGRITY_CHECK_FAILED) notify(QMessageBox::Critical, ERROR_TITLE, DECRYPT_ERROR, INTEGRITY_ERROR);
        else    // Some other odd exception
        {
            notify(QMessageBox::Warning, ERROR_TITLE, DECRYPT_ERROR, QString(ex.what()));
            this->clean();
            this->hide();
        }
        return false;
    }
    setStatus(COMPLETE);
    return true;
}

void Authenticator::setStatus(const QString& status) { statusBar()->showMessage(status); }  // Set authenticator status

void Authenticator::updateYubiKeyState() { yubikeyState->setText(yubikey->stateText()); }   // Report current YubiKey state

void Authenticator::on_masterPasswordLineEdit_textEdited(const QString &arg1)
{
    setStatus(WAITING);
    if (arg1.length() >= 8)
    {
        ui->challengeButton->setEnabled(true);
        canChallenge = true;
    }
    else
    {
        ui->challengeButton->setEnabled(false);
        canChallenge = false;
    }
}

void Authenticator::on_challengeButton_clicked() { formKey(); } // Trigger a challenge via button

void Authenticator::on_masterPasswordLineEdit_returnPressed() { formKey(); }    // Trigger a challenge via textbox

int Authenticator::notify(QMessageBox::Icon icon, const QString& title, const QString& text, const QString& detailText)  // Notify user of some issue
{
    QMessageBox msg;
    msg.setWindowTitle(title);
    msg.setIcon(icon);
    msg.setText(text);
    msg.setInformativeText(detailText);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    return msg.exec();
}

void Authenticator::on_slotOneRadioButton_clicked() { yubikey->setSlot(YubiKey::SLOT_ONE); }

void Authenticator::on_slotTwoRadioButton_clicked() { yubikey->setSlot(YubiKey::SLOT_TWO); }
