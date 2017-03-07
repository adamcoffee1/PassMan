/*
 * Description: Implementation of the YubiKey class.
 *              Abstracts lower-level operations with Yubico's binaries for YubiKey interaction.
 *              Uses binaries from the 'yubikey-personalization' package, including 'ykchalresp' and 'ykinfo'.
 *              Allows for HMAC-SHA1 challenge-responses and metadata gathering on connected YubiKeys.
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#include "yubikey.h"

const QString YubiKey::HMAC_SLOT_1_COMMAND = "ykchalresp -1 -x -i-";    // Common values
const QString YubiKey::HMAC_SLOT_2_COMMAND = "ykchalresp -2 -x -i-";
const QString YubiKey::GET_SERIAL_COMMAND = "ykinfo -s";
const QString YubiKey::GET_VERSION_COMMAND = "ykinfo -v";
const QString YubiKey::GET_USB_COMMAND = "lsusb";
const QString YubiKey::USB_NAME = "Yubikey";
const QString YubiKey::YUBIKEY_TIMEOUT = "Yubikey core error: timeout\n";
const QString YubiKey::YUBIKEY_NOT_PRESENT = "Yubikey core error: no yubikey present\n";
const QString YubiKey::DEVICE_WATCH_PATH = "/dev/";
const QString YubiKey::USB_WATCH_PATH = "/dev/usb/";
const QString YubiKey::PRESENT_MSG = "YubiKey connected";   // Common state messages
const QString YubiKey::TIMEOUT_MSG = "YubiKey timeout";
const QString YubiKey::NOT_PRESENT_MSG = "YubiKey not connected";
const QString YubiKey::UNKNOWN_MSG = "";
const QString YubiKey::UNKNOWN_ERROR_MSG = "YubiKey error";
const int YubiKey::SLOT_ONE = 1;
const int YubiKey::SLOT_TWO = 2;
const int YubiKey::MAX_HMAC_CHALLENGE_SIZE = 64;

YubiKey::YubiKey()
{
    lastState = UNKNOWN;
    slot = 1;
    watcher = new QFileSystemWatcher(); // Watch for changes to /dev/ directory
    QObject::connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(deviceChange()));
    usbWatcher = new QFileSystemWatcher();  // Watch for changes specifically to /dev/usb/ directory to check for YubiKeys
    QObject::connect(usbWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(usbChange()));
    watcher->addPath(DEVICE_WATCH_PATH);
}

YubiKey::~YubiKey()
{
    delete watcher;
    delete usbWatcher;
}

QByteArray YubiKey::hmacSHA1(const QByteArray& challenge, bool blocking)    // Complete an HMAC-SHA1 challenge-response
{
    QProcess* proc = new QProcess();    // Will run Yubico software in separate process
    proc->start(slot == 1 ? HMAC_SLOT_1_COMMAND : HMAC_SLOT_2_COMMAND, QIODevice::ReadWrite);
    proc->write(challenge.toHex()); // Send challenge via standard input
    proc->closeWriteChannel();
    if (blocking) proc->waitForFinished(-1);    // YubiKey may require button-press, wait if caller desired
    QString error(proc->readAllStandardError());
    QString out(proc->readAllStandardOutput());
    setState(error, out);
    proc->close();
    delete proc;
    return out.left(out.length() - 1).toUtf8(); // Strip newline
}

int YubiKey::state() { return lastState; }  // Return current state of the YubiKey

QString YubiKey::serial() // Return decimal serial number of the YubiKey
{
    QProcess* proc = new QProcess();    // Will run Yubico software in separate process
    proc->start(GET_SERIAL_COMMAND, QIODevice::ReadWrite);
    proc->waitForFinished(-1);
    QString error(proc->readAllStandardError());
    QString out(proc->readAllStandardOutput());
    setState(error, out);
    proc->close();
    delete proc;
    return out.left(out.length() - 1).right(7); // Strip newline
}

QString YubiKey::version() // Return version of the YubiKey
{
    QProcess* proc = new QProcess();    // Will run Yubico software in separate process
    proc->start(GET_VERSION_COMMAND, QIODevice::ReadWrite);
    proc->waitForFinished(-1);
    QString error(proc->readAllStandardError());
    QString out(proc->readAllStandardOutput());
    setState(error, out);
    proc->close();
    delete proc;
    return out.left(out.length() - 1).right(5); // Strip newline
}

void YubiKey::setState(const QString& error, const QString& out) // Interpret the state of the YubiKey after an operation attempt
{
    if (!error.compare(YUBIKEY_TIMEOUT)) lastState = TIMEOUT;   // Save resulting state of operation
    else if (!error.compare(YUBIKEY_NOT_PRESENT)) lastState = NOT_PRESENT;
    else if (!out.compare("")) lastState = UNKNOWN_ERROR;
    else lastState = PRESENT;
}

QString YubiKey::stateText() // Return the description of the current state
{
    switch (lastState)
    {
        case PRESENT: return PRESENT_MSG;
        case TIMEOUT: return TIMEOUT_MSG;
        case NOT_PRESENT: return NOT_PRESENT_MSG;
        case UNKNOWN: return UNKNOWN_MSG;
        case UNKNOWN_ERROR: return UNKNOWN_ERROR_MSG;
    }
    return "";
}

void YubiKey::poll() { version(); } // Query any YubiKey to acquire status

int YubiKey::currSlot() { return slot; }    // Return current config slot

void YubiKey::setSlot(int s) { if (slot == 1 || slot == 2) slot = s; }  // Set the config slot

void YubiKey::deviceChange() { if (usbWatcher->files().length() < 1) usbWatcher->addPath(USB_WATCH_PATH); } // Check if a USB device change occured

void YubiKey::usbChange()   // Check if USB change was a YubiKey change
{
    QProcess* proc = new QProcess();    // Will run listing program in separate process
    proc->start(GET_USB_COMMAND, QIODevice::ReadWrite);
    proc->waitForFinished(-1);
    QString out(proc->readAllStandardOutput());
    if (out.contains(USB_NAME)) this->poll();
    else lastState = NOT_PRESENT;
    proc->close();
    delete proc;
    emit yubiKeyChanged();  // Notify watchers that a change has occured
}
