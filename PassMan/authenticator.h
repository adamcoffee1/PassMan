/*
 * Description: Definition of the Authenticator class.
 *              Handles secure file encryption/decryption operations.
 *              Utilizes AES-256 in GCM-AE mode.
 *              Two factors are used for the key: A user password, and their YubiKey's HMAC-SHA1 response.
 *              They are combined to a single master key via PBKDF2-SHA512.
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <QMainWindow>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <crypto++/osrng.h>
#include <crypto++/filters.h>
#include <crypto++/aes.h>
#include <crypto++/gcm.h>
#include <crypto++/cryptlib.h>
#include <crypto++/pwdbased.h>
#include "yubikey.h"
#include "database.h"
#include <QDebug> //TESTING!

namespace Ui
{
    class Authenticator;
}

class Authenticator : public QMainWindow
{
    Q_OBJECT

    public:
        explicit Authenticator(YubiKey* yk, QWidget *parent = 0);
        ~Authenticator();

        void open(const QString& filename, Database* db);    // Decrypt a file
        void save(const QString& filename, Database* db);    // Encrypt a file
        void clean();   // Reset authenticator and wipe any sensitive data

    private slots:
        void updateYubiKeyState();  // Report current YubiKey state
        void on_masterPasswordLineEdit_textEdited(const QString &arg1);
        void on_challengeButton_clicked();
        void on_masterPasswordLineEdit_returnPressed();
        void on_slotOneRadioButton_clicked();
        void on_slotTwoRadioButton_clicked();

private:
        static const char FILE_PORTION_SEPARATOR;   // Commonly used values
        static const double MIN_PBKDF_TIME;
        static const int TAG_SIZE, DECRYPT_MODE, ENCRYPT_MODE;
        static const int IV_SIZE = CryptoPP::AES::BLOCKSIZE * 16;   // Bytes in tag for GCM-AE
        static const int SALT_SIZE = 16;
        static const QString WAITING, BUSY_YUBIKEY, BUSY_KEY, COMPLETE, FAILED, ERROR_TITLE, ENCRYPT_ERROR, DECRYPT_ERROR,
                             DB_ERROR, FILE_ERROR, PIECES_ERROR, HMAC_ERROR, IV_ERROR, CIPHER_ERROR, INTEGRITY_ERROR,
                             YUBIKEY_ERROR, YUBIKEY_HMAC_ERROR, YUBIKEY_PRESENT_ERROR, SALT_ERROR, ITERATION_ERROR;
        Ui::Authenticator *ui;
        YubiKey* yubikey;
        Database* db;
        QLabel* yubikeyState;
        bool canChallenge;
        QString fileName;
        bool operationMode; // Whether in decryption or encryption mode

        byte key[CryptoPP::AES::MAX_KEYLENGTH]; // Crypto-related values
        byte iv[IV_SIZE];
        byte salt[SALT_SIZE];
        int iterations;
        QByteArray challenge;
        QByteArray response;
        std::string clear;
        std::string cipher;

        void formKey(); // Create master key and do operation
        int encrypt();  // Perform authenticated AES-256 encryption in GCM-AE mode
        int decrypt();  // Perform authenticated AES-256 decryption in GCM-AE mode
        void setStatus(const QString& status);  // Set authenticator status
        int notify(QMessageBox::Icon, const QString& title, const QString& text, const QString& detailText);    // Notify user of some issue
};

#endif // AUTHENTICATOR_H
