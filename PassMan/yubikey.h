/*
 * Description: Definition of the YubiKey class.
 *              Abstracts lower-level operations with Yubico's binaries for YubiKey interaction.
 *              Uses binaries from the 'yubikey-personalization' package, including 'ykchalresp' and 'ykinfo'.
 *              Allows for HMAC-SHA1 challenge-responses and metadata gathering on connected YubiKeys.
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#ifndef YUBIKEY_H
#define YUBIKEY_H

#include <QString>
#include <QProcess>
#include <QFileSystemWatcher>
#include <QDir>

class YubiKey : public QObject
{
    Q_OBJECT

    public:
        enum State { PRESENT, TIMEOUT, NOT_PRESENT, UNKNOWN, UNKNOWN_ERROR };   // Possible states
        static const int SLOT_ONE, SLOT_TWO, MAX_HMAC_CHALLENGE_SIZE;

        YubiKey();
        ~YubiKey();

        QByteArray hmacSHA1(const QByteArray& challenge, bool blocking);    // Complete an HMAC-SHA1 challenge-response
        int state();    // Return current state of YubiKey
        QString serial();   // Return decimal serial number of the YubiKey
        QString version();  // Return version of the YubiKey
        QString stateText();    // Return the description of the current state
        void setSlot(int s);    // Set the config slot
        int currSlot(); // Return current config slot
        void poll();    // Query any YubiKey to acquire status

    signals:
        void yubiKeyChanged();  // Signal that a YubiKey may have been inserted/removed

    private slots:
        void deviceChange();    // Check if a USB device change occured
        void usbChange();   // Check if USB change was a YubiKey change

    private:
        static const QString HMAC_SLOT_1_COMMAND, HMAC_SLOT_2_COMMAND, GET_SERIAL_COMMAND, GET_VERSION_COMMAND, // Common values
                             GET_USB_COMMAND, USB_NAME, YUBIKEY_TIMEOUT, YUBIKEY_NOT_PRESENT,
                             DEVICE_WATCH_PATH, USB_WATCH_PATH;
        static const QString PRESENT_MSG, TIMEOUT_MSG, NOT_PRESENT_MSG, UNKNOWN_MSG, UNKNOWN_ERROR_MSG; // Common state messages
        QFileSystemWatcher* watcher, * usbWatcher;
        int lastState;
        int slot;

        void setState(const QString& error, const QString& out);    // Interpret the state of the YubiKey after an operation attempt
};

#endif // YUBIKEY_H
