/*
 * Description: Definition of the PassMan class.  Manages GUI interaction and visual consistency.
 *              Provides two-factor secured password manager interface and associated operations.
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#ifndef PASSMAN_H
#define PASSMAN_H

#include <QMainWindow>
#include <QMessageBox>
#include <QString>
#include <QLabel>
#include <QFileDialog>
#include <QFile>
#include <QIODevice>
#include <QProcess>
#include <QJsonDocument>
#include "database.h"
#include "yubikeytester.h"
#include "yubikey.h"
#include "authenticator.h"
#include "strengthcalculator.h"
#include "about.h"
#include "help.h"
#include "generator.h"
#include <QDebug> //TESTING!!

namespace Ui
{
    class PassMan;
}

class PassMan : public QMainWindow
{
    Q_OBJECT

    public:
        explicit PassMan(QWidget *parent = 0);
        ~PassMan();

    private slots:
        void on_revealPasswordCheckBox_toggled(bool checked);
        void on_actionOpen_Database_triggered();
        void on_actionNew_Database_triggered();
        void on_actionSave_Database_triggered();
        void on_actionSaveas_Database_triggered();
        void on_notesTextEdit_textChanged();
        void on_actionClose_Database_triggered();
        void on_actionQuit_triggered();
        void on_actionDelete_Entry_triggered();
        void on_actionAdd_Entry_triggered();
        void on_actionAbout_triggered();
        void on_actionHow_to_Use_triggered();
        void on_actionYubiKey_Tester_triggered();
        void on_entryTableWidget_itemSelectionChanged();
        void on_entryNameLineEdit_textEdited(const QString &arg1);
        void on_usernameLineEdit_textEdited(const QString &arg1);
        void on_passwordLineEdit_textEdited(const QString &arg1);
        void on_repeatedPasswordLineEdit_textEdited(const QString &arg1);
        void updateStatusInfo();    // Update status bar
        void fileReadDone();    // Update GUI and states after file operation
        void fileWriteDone();   // Update state after file operation
        void passGenDone(); // Update GUI after password generation
        void on_actionPassword_Generator_triggered();
        void on_generatePasswordButton_clicked();
        void on_actionCopy_Entry_Username_triggered();
        void on_actionCopy_Entry_Password_triggered();
        void on_actionPassword_Strength_Calculator_triggered();
        void on_actionAbout_Qt_triggered();
        void on_actionAuto_Type_Entry_triggered();

private:
        static const QString VERSION, NOT_LOADED, LOADED, FILE_FILTER, FILE_EXTENSION,  // Commonly used values
                             CLOSE_TITLE, CLOSE_QUESTION, OPEN_EXISTING_TITLE, CREATE_NEW_TITLE,
                             SAVE_AS_TITLE, LINEEDIT_WHITE_BG, LINEEDIT_YELLOW_BG;
        Ui::PassMan *ui;
        Database *db;
        QLabel* yubikeyState;
        YubiKey* yubikey;
        YubiKeyTester* tester;
        Authenticator* auth;
        Generator* gen;
        About* about;
        Help* help;
        StrengthCalculator* strength;
        bool passMismatch, isOpen, isSaved;  // Indicate program state
        QString fileName;

        void open(bool existing);   // Open a database file
        void save(bool existing);   // Save a database file
        bool close();    // Handle possible database closing
        void configGUI();  // Initialize GUI components for proper interaction
        void updateActions();   // Toggle menu actions based on program state
        int confirmClose(QString title, QString text);  // Confirm via message box whether to close
        void updateListInfo(int row);   // Update the entry list after database change, or refresh if negative
        void updateDisplayInfo(int row);    // Update the textboxes with currently selected entry, or clear if negative
        int selectedItem(); // Returns currently selected item in the entry list
        void updatePasswords(); // Handle parity between password textboxes on text changes
        void closeEvent(QCloseEvent*);  // Handle window closing without leaking data
};

#endif // PASSMAN_H
