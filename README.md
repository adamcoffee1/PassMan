# PassMan
### *A two-factor encrypted password manager*

## Features
* YubiKey challenge-response as second factor
* Full-database optimized authenticated encryption
* Key-stretching for master key generation
* Auto-type functionality for login form interaction
* Customizable password generator and strength calculator

## Security
PassMan utilizes the [Crypto++](https://www.cryptopp.com/) library for secure operations, including key generation, encryption, and decryption.  Two factors are combined to create the master key that secures the database file:

1. The user's master password (ideally a long password they must remember)
2. The user's YubiKey (preset with a unique HMAC key)

Specifically, the master password is concatenated with the YubiKey's 20-byte [HMAC-SHA1](https://en.wikipedia.org/wiki/Hash-based_message_authentication_code) response to a random 64-byte challenge.  A 32-byte key is then derived via PBKDF2 with SHA512 and a 16-byte random salt.  AES-256 is used with a random 256-byte initialization vector in GCM-AE mode to provide authenticated encryption of the entire file.  All sensitive variables are wiped from memory prior to exiting the application, or after closing a database.

## YubiKey Configuration
You must have a YubiKey with one configuration slot set to HMAC-SHA1.  This can be done through Yubico's YubiKey Personalization Tool, available as the package *yubikey-personalization-gui*.  Here's an example of the correct tab - be sure to generate a unique Secret Key:

![Personalization](/screenshots/YubiKeyPersonalization.png)

## Screenshots
The main window, where account entries can be edited and auto-typed:

![Main Window](/screenshots/MainWindow.png)


The customizable password generator:

![Generator](/screenshots/Generator.png)


And testing functionality for the YubiKey:

![YubiKey](/screenshots/YubiKey.png)

## Usage
PassMan simply stores your account passwords and other information in an encrypted database file.  This is kept on your storage and is never transmitted elsewhere.  If you wish to access your database across many devices, consider keeping the *.pmdb* database file on a cloud file service like Dropbox.

To start, simply create a new database and begin adding your account entries.  When saving the database, you'll be prompted for a master password.  Make this strong - it's the only password you'll now need to remember!  Your YubiKey will then be challenged to obtain its response as the second encryption factor.  See this [video](https://www.youtube.com/watch?v=BNIZxAZJLts) for a demonstration of usage.

## Installation
While PassMan is designed in Qt, in its current form it is only functional on Linux.  This is due to the implementation of YubiKey detection and Yubico software used to query it.

The following are required to compile and run PassMan:

1. [yubikey-personalization](https://developers.yubico.com/yubikey-personalization/)
2. [crypto++](https://www.cryptopp.com)
3. [Qt](http://doc.qt.io/qt-5/)
4. [xdotool](http://www.semicomplete.com/projects/xdotool)

To install, download the latest of [installer](/install/) files.  Untar the file, then enable execution of the included shell script and run it.  You may be prompted to install the aforementioned dependencies.  See this [video](https://www.youtube.com/watch?v=nsx8m-WDR2M) for a demonstration of installation.

## License
Licensed under the three-clause BSD license, found in the [LICENSE](/PassMan/LICENSE) file.
