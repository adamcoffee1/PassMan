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
2. The user's YubiKey (HMAC response unique to this device)

Specifically, the master password is concatenated with the YubiKey's 20-byte [HMAC-SHA1](https://en.wikipedia.org/wiki/Hash-based_message_authentication_code) response to a random 64-byte challenge.  A 32-byte key is then derived via PBKDF2 with SHA512 and a 16-byte random salt.  AES-256 is used with a random 256-byte initialization vector in GCM-AE mode to provide authenticated encryption of the entire file.  All sensitive variables are wiped from memory prior to exiting the application, or after closing a database.

## Screenshots
The main window, where account entries can be edited and auto-typed:

![Main Window](/screenshots/Main Window.png)


The customizable password generator:

![Generator](/screenshots/Generator.png)


And testing functionality for the YubiKey:

![YubiKey](/screenshots/YubiKey.png)

## License
Licensed under the three-clause BSD license, found in the [LICENSE](/PassMan/LICENSE) file.
