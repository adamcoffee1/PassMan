/*
 * Description: Implementation of the YubiKeyTester class.
 *              This class provides a testing interface for the HMAC-SHA1 challenge-response mechanism.
 *              YubiKey metadata is also shown for diagnostics.
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#include "yubikeytester.h"
#include "ui_yubikeytester.h"

const QString YubiKeyTester::WAITING = "Waiting to challenge"; // Define commonly used values
const QString YubiKeyTester::BUSY = "Contacting YubiKey";
const QString YubiKeyTester::FAILED = "Failed";
const QString YubiKeyTester::COMPLETE = "Received response";

YubiKeyTester::YubiKeyTester(YubiKey* yk, QWidget *parent) : QMainWindow(parent), ui(new Ui::YubiKeyTester)
{
    canChallenge = false;
    yubikey = yk;
    ui->setupUi(this);
    connect(yubikey, SIGNAL(yubiKeyChanged()), this, SLOT(updateDetails()));  // Update details if a YubiKey may have been plugged in
    setStatus(WAITING);
    yubikeyState = new QLabel();
    statusBar()->addPermanentWidget(yubikeyState);
    statusBar()->addPermanentWidget(new QLabel(" "));   // Dummy label to add space on right of statusBar
    yubikey->poll();
    updateDetails();
}

YubiKeyTester::~YubiKeyTester() { delete ui; }

void YubiKeyTester::on_sendButton_clicked() { challenge(); }    // Trigger a challenge via button

void YubiKeyTester::on_challengeLineEdit_returnPressed() { challenge(); }   // Trigger a challenge via textbox

void YubiKeyTester::on_challengeLineEdit_textChanged(const QString &arg1) // Restrict challenges if nothing entered
{
    setStatus(WAITING);
    if (arg1.length() > 0)
    {
        ui->sendButton->setEnabled(true);
        canChallenge = true;
    }
    else
    {
        ui->sendButton->setEnabled(false);
        canChallenge = false;
    }
}

void YubiKeyTester::on_slotOneRadioButton_clicked() // Maintain status consistency
{
    yubikey->setSlot(YubiKey::SLOT_ONE);
    setStatus(WAITING);
}

void YubiKeyTester::on_slotTwoRadioButton_clicked() // Maintain status consistency
{
    yubikey->setSlot(YubiKey::SLOT_TWO);
    setStatus(WAITING);
}

QByteArray YubiKeyTester::getChallenge() { return ui->challengeLineEdit->text().toUtf8(); } // Retrieve current entered challenge

void YubiKeyTester::setResponse(const QString& response) { ui->responseLineEdit->setText(response); }   // Display response

void YubiKeyTester::setStatus(const QString& status) { statusBar()->showMessage(status); } // Set tester status

void YubiKeyTester::challenge() // Initiate an HMAC challenge
{
    if (canChallenge)
    {
        setStatus(BUSY);
        setResponse(yubikey->hmacSHA1(getChallenge(), true));
        yubikeyState->setText(yubikey->stateText());
        yubikey->state() == YubiKey::PRESENT ? setStatus(COMPLETE) : setStatus(FAILED);
    }
}

void YubiKeyTester::updateDetails() // Get data about YubiKey
{
    QString serial(yubikey->serial());
    QString version(yubikey->version());
    yubikeyState->setText(yubikey->stateText());
    if (yubikey->state() == YubiKey::PRESENT)
    {
        ui->serialNumberLineEdit->setText(yubikey->serial());
        ui->versionLineEdit->setText(yubikey->version());
        setStatus(WAITING);
    }
    else
    {
        ui->serialNumberLineEdit->clear();
        ui->versionLineEdit->clear();
    }
}
