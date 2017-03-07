/*
 * Description: Definition of the YubiKeyTester class.
 *              This class provides a testing interface for the HMAC-SHA1 challenge-response mechanism.
 *              YubiKey metadata is also shown for diagnostics.
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#ifndef YUBIKEYTESTER_H
#define YUBIKEYTESTER_H

#include <QMainWindow>
#include <QString>
#include <QLabel>
#include "yubikey.h"

namespace Ui
{
    class YubiKeyTester;
}

class YubiKeyTester : public QMainWindow
{
    Q_OBJECT

    public:
        explicit YubiKeyTester(YubiKey* yk, QWidget *parent = 0);
        ~YubiKeyTester();

    private slots:
        void updateDetails();   // Get data about YubiKey
        void on_sendButton_clicked();   // Trigger a challenge via button
        void on_challengeLineEdit_returnPressed();  // Trigger a challenge via textbox
        void on_challengeLineEdit_textChanged(const QString &arg1); // Restrict challenges if nothing entered
        void on_slotOneRadioButton_clicked();   // Maintain status consistency
        void on_slotTwoRadioButton_clicked();   // Maintain status consistency

    private:
        static const QString WAITING, BUSY, FAILED, COMPLETE;  // Commonly used values
        Ui::YubiKeyTester *ui;
        YubiKey* yubikey;
        QLabel* yubikeyState;
        bool canChallenge;

        QByteArray getChallenge(); // Retrieve current entered challenge
        void setResponse(const QString& response);  // Display response
        void setStatus(const QString& status);  // Set tester status
        void challenge();   // Initiate an HMAC challenge
};

#endif // YUBIKEYTESTER_H
