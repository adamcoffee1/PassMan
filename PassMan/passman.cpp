/*
 * Description: Implementation of the PassMan class.  Manages GUI interaction and visual consistency.
 *              Provides two-factor secured password manager interface and associated operations.
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#include "passman.h"
#include "ui_passman.h"

const QString PassMan::VERSION = "PassMan 1.3.4";
const QString PassMan::NOT_LOADED = "No database loaded";
const QString PassMan::LOADED = "Database loaded";
const QString PassMan::FILE_FILTER = "Passman Database (*.pmdb)";
const QString PassMan::FILE_EXTENSION = ".pmdb";
const QString PassMan::CLOSE_TITLE = "Close PassMan Database";
const QString PassMan::CLOSE_QUESTION = "Save changes to database before closing?";
const QString PassMan::OPEN_EXISTING_TITLE = "Open Existing PassMan Database";
const QString PassMan::CREATE_NEW_TITLE = "Create New PassMan Database";
const QString PassMan::SAVE_AS_TITLE = "Save as New PassMan Database";
const QString PassMan::LINEEDIT_WHITE_BG = "QLineEdit {}";
const QString PassMan::LINEEDIT_YELLOW_BG = "QLineEdit {background-color: yellow;}";

PassMan::PassMan(QWidget *parent) : QMainWindow(parent), ui(new Ui::PassMan)
{
    db = new Database(VERSION);
    yubikey = new YubiKey();
    gen = new Generator();
    connect(yubikey, SIGNAL(yubiKeyChanged()), this, SLOT(updateStatusInfo()));
    connect(db, SIGNAL(readNewData()), this, SLOT(fileReadDone()));
    connect(db, SIGNAL(writeNewData()), this, SLOT(fileWriteDone()));
    connect(gen, SIGNAL(passwordGenerated()), this, SLOT(passGenDone()));
    tester = new YubiKeyTester(yubikey);
    auth = new Authenticator(yubikey);
    about = new About(VERSION);
    help = new Help();
    strength = new StrengthCalculator();
    passMismatch = false;
    isSaved = true;
    isOpen = false;
    configGUI();
}

PassMan::~PassMan()
{
    tester->hide(); // Don't leave other windows hanging around!
    auth->hide();
    about->hide();
    gen->hide();
    strength->hide();
    help->hide();
    delete db;
    delete ui;
    delete tester;
    delete yubikey;
    delete auth;
    delete about;
    delete gen;
    delete strength;
}

void PassMan::fileReadDone()    // Update GUI and states after file operation
{
    isOpen = isSaved = true;
    updateListInfo(-1);
    updateActions();
}

void PassMan:: fileWriteDone() { isSaved = true; }  // Update state after file operation

void PassMan::open(bool existing)   // Open a database file
{
    QString filter(FILE_FILTER);
    if (existing)
    {
        fileName = QFileDialog::getOpenFileName(ui->passManCentralWidget, OPEN_EXISTING_TITLE, "", filter, &filter);
        if (fileName.length() < 1) return;  // Failed to get filename (user cancelled)
        auth->open(fileName, db);
    }
    else    // Make new database file
    {
        fileName = QFileDialog::getSaveFileName(ui->passManCentralWidget, CREATE_NEW_TITLE, "", filter, &filter);
        if (fileName.length() < 1) return;  // Failed to get filename (user cancelled)
        if (!fileName.endsWith(FILE_EXTENSION)) fileName.append(FILE_EXTENSION);
        fileReadDone();
    }
}

void PassMan::save(bool existing)   // Save a database file
{
    QString filter(FILE_FILTER);
    QFile file;
    if (!existing)  // Make new database file
    {
        fileName = QFileDialog::getSaveFileName(ui->passManCentralWidget, SAVE_AS_TITLE, "", filter, &filter);
        if (fileName.length() < 1) return;   // Failed to get filename (user cancelled)
        if (!fileName.endsWith(FILE_EXTENSION)) fileName.append(FILE_EXTENSION);
    }
    file.setFileName(fileName);
    auth->save(fileName, db);
}

void PassMan::configGUI()   // Initialize GUI components for proper interaction
{
    ui->setupUi(this);
    ui->actionAdd_Entry->setShortcut(QKeySequence::New);   // Add keyboard shortcuts for actions
    ui->actionAuto_Type_Entry->setShortcut(QKeySequence::Paste);
    ui->actionClose_Database->setShortcut(QKeySequence::Close);
    ui->actionCopy_Entry_Username->setShortcut(QKeySequence::Underline);
    ui->actionCopy_Entry_Password->setShortcut(QKeySequence::Copy);
    ui->actionDelete_Entry->setShortcuts(QKeySequence::Delete);
    ui->actionHow_to_Use->setShortcut(QKeySequence::HelpContents);
    ui->actionOpen_Database->setShortcut(QKeySequence::Open);
    ui->actionSave_Database->setShortcut(QKeySequence::Save);
    ui->actionSaveas_Database->setShortcut(QKeySequence::SaveAs);
    ui->actionQuit->setShortcut(QKeySequence::Quit);
    ui->entryTableWidget->setColumnCount(1);
    ui->entryTableWidget->verticalHeader()->setVisible(false);  // Alter entry table to look cleaner and have simple interaction
    ui->entryTableWidget->horizontalHeader()->setVisible(false);
    ui->entryTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->entryTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->entryTableWidget->setShowGrid(false);
    ui->entryTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->entryTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);     // By default, keep passwords obscured
    ui->repeatedPasswordLineEdit->setEchoMode(QLineEdit::Password);
    yubikeyState = new QLabel();
    statusBar()->addPermanentWidget(yubikeyState);
    statusBar()->addPermanentWidget(new QLabel(" "));   // Dummy label to add space on right of statusBar
    ui->passwordStrengthBar->setMinimum(0);
    ui->passwordStrengthBar->setMaximum(StrengthCalculator::NAIVE_HIGH_STRENGTH_ENTROPY);
    ui->passwordStrengthBar->setFormat("%v bits");
    yubikey->poll();
    updateStatusInfo();
}

void PassMan::updateActions()   // Toggle menu actions based on program state
{
    if (isOpen)
    {
        statusBar()->showMessage(LOADED);
        ui->actionAdd_Entry->setEnabled(true);
        ui->actionNew_Database->setEnabled(false);
        ui->actionOpen_Database->setEnabled(false);
        ui->actionSaveas_Database->setEnabled(true);
        ui->actionSave_Database->setEnabled(true);
        ui->actionClose_Database->setEnabled(true);
        if (db->size() > 0)
        {
            ui->actionCopy_Entry_Username->setEnabled(true);
            ui->actionCopy_Entry_Password->setEnabled(true);
            ui->actionAuto_Type_Entry->setEnabled(true);
            ui->actionDelete_Entry->setEnabled(true);
            ui->entryNameLineEdit->setEnabled(true);
            ui->usernameLineEdit->setEnabled(true);
            ui->passwordLineEdit->setEnabled(true);
            ui->repeatedPasswordLineEdit->setEnabled(true);
            ui->notesTextEdit->setEnabled(true);
            ui->generatePasswordButton->setEnabled(true);
            ui->revealPasswordCheckBox->setEnabled(true);
        }
        else
        {
            ui->actionCopy_Entry_Username->setEnabled(false);
            ui->actionCopy_Entry_Password->setEnabled(false);
            ui->actionAuto_Type_Entry->setEnabled(false);
            ui->actionDelete_Entry->setEnabled(false);
            ui->entryNameLineEdit->setEnabled(false);
            ui->usernameLineEdit->setEnabled(false);
            ui->passwordLineEdit->setEnabled(false);
            ui->repeatedPasswordLineEdit->setEnabled(false);
            ui->notesTextEdit->setEnabled(false);
            ui->generatePasswordButton->setEnabled(false);
            ui->revealPasswordCheckBox->setEnabled(false);
        }
    }
    else
    {
        statusBar()->showMessage(NOT_LOADED);
        ui->actionCopy_Entry_Username->setEnabled(false);
        ui->actionCopy_Entry_Password->setEnabled(false);
        ui->actionAdd_Entry->setEnabled(false);
        ui->actionAuto_Type_Entry->setEnabled(false);
        ui->actionDelete_Entry->setEnabled(false);
        ui->actionNew_Database->setEnabled(true);
        ui->actionOpen_Database->setEnabled(true);
        ui->actionSaveas_Database->setEnabled(false);
        ui->actionSave_Database->setEnabled(false);
        ui->actionClose_Database->setEnabled(false);
        ui->entryNameLineEdit->setEnabled(false);
        ui->usernameLineEdit->setEnabled(false);
        ui->passwordLineEdit->setEnabled(false);
        ui->repeatedPasswordLineEdit->setEnabled(false);
        ui->notesTextEdit->setEnabled(false);
        ui->generatePasswordButton->setEnabled(false);
        ui->revealPasswordCheckBox->setEnabled(false);
    }
}

void PassMan::on_revealPasswordCheckBox_toggled(bool checked)   // Toggle revealing of password textboxes via checkbox
{
    if (checked)
    {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
        ui->repeatedPasswordLineEdit->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
        ui->repeatedPasswordLineEdit->setEchoMode(QLineEdit::Password);
    }
}

void PassMan::on_entryNameLineEdit_textEdited(const QString &arg1) // Update entry name if changed
{
    isSaved = false;
    int row = selectedItem();
    db->setName(arg1, row);
    updateListInfo(row);
}

void PassMan::on_actionOpen_Database_triggered() { open(true); }    // Open an existing database file

void PassMan::on_entryTableWidget_itemSelectionChanged()    // Update rest of GUI with data corresponding with this entry
{
    passMismatch = false;
    ui->repeatedPasswordLineEdit->setStyleSheet(LINEEDIT_WHITE_BG);
    ui->passwordLineEdit->setStyleSheet(LINEEDIT_WHITE_BG);
    updateDisplayInfo(selectedItem());
    int strength = StrengthCalculator::naiveEntropyBits(ui->passwordLineEdit->text());
    if (ui->passwordStrengthBar->maximum() < strength) ui->passwordStrengthBar->setMaximum(strength);
    ui->passwordStrengthBar->setValue(strength);
}

void PassMan::on_actionNew_Database_triggered() { open(false); }    // Create a new database file

void PassMan::on_actionSave_Database_triggered() { save(true); }    // Save current database file

void PassMan::on_actionSaveas_Database_triggered() { save(false); } // Save current database file with new name

void PassMan::on_notesTextEdit_textChanged()    // Update entry notes if changed
{
    isSaved = false;
    db->setNotes(ui->notesTextEdit->toPlainText(), selectedItem());
}

void PassMan::on_usernameLineEdit_textEdited(const QString &arg1)  // Update entry username if changed
{
    isSaved = false;
    db->setUsername(ui->usernameLineEdit->text(), selectedItem());
}

void PassMan::on_passwordLineEdit_textEdited(const QString &arg1)   // Update entry password if changed
{
    int strength = StrengthCalculator::naiveEntropyBits(ui->passwordLineEdit->text());
    if (ui->passwordStrengthBar->maximum() < strength) ui->passwordStrengthBar->setMaximum(strength);
    ui->passwordStrengthBar->setValue(strength);
    updatePasswords();
}

void PassMan::on_repeatedPasswordLineEdit_textEdited(const QString &arg1)
{
    int strength = StrengthCalculator::naiveEntropyBits(ui->repeatedPasswordLineEdit->text());
    if (ui->passwordStrengthBar->maximum() < strength) ui->passwordStrengthBar->setMaximum(strength);
    ui->passwordStrengthBar->setValue(strength);
    updatePasswords();
}

void PassMan::updatePasswords()    // Handle parity between password textboxes on text changes
{
    if (!ui->passwordLineEdit->text().compare(ui->repeatedPasswordLineEdit->text()))    // Match!
    {
        passMismatch = false;
        ui->passwordLineEdit->setStyleSheet(LINEEDIT_WHITE_BG);
        ui->repeatedPasswordLineEdit->setStyleSheet(LINEEDIT_WHITE_BG);
        isSaved = false;
        db->setPassword(ui->passwordLineEdit->text(), selectedItem());
    }
    else if (!passMismatch) // Mismatch!
    {
        ui->passwordLineEdit->setStyleSheet(LINEEDIT_YELLOW_BG);
        passMismatch = true;
    }
}

void PassMan::updateStatusInfo()    // Update status bar
{
    yubikeyState->setText(yubikey->stateText());
    if (isOpen) statusBar()->showMessage(LOADED);
    else statusBar()->showMessage(NOT_LOADED);
}

int PassMan::confirmClose(QString title, QString text)  // Confirm via message box whether to close
{
    QMessageBox msg;
    msg.setWindowTitle(title);
    msg.setIcon(QMessageBox::Question);
    msg.setText(text);
    msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Save);
    return msg.exec();
}

void PassMan::updateListInfo(int row)   // Update the entry list after database change, or refresh if negative
{
    if (row < 0)
    {
        ui->entryTableWidget->clear();
        ui->entryTableWidget->setColumnCount(1);
        ui->entryTableWidget->setRowCount(db->size());  // Update entire entry list
        for (int i = 0; i < db->size(); i++)
        {
            ui->entryTableWidget->setItem(i, 0, new QTableWidgetItem(db->name(i)));
        }
        if (ui->entryTableWidget->rowCount() > 0)
        {
            ui->entryTableWidget->selectRow(0); // Want to select first item on fresh load
            row = 0;
        }
    }   // Select the current row if valid
    if (ui->entryTableWidget->rowCount() > 0) ui->entryTableWidget->setItem(row, 0, new QTableWidgetItem(db->name(row)));
    updateDisplayInfo(row);
}

void PassMan::updateDisplayInfo(int row)   // Update the textboxes with currently selected entry, or clear if negative
{
    if (row < 0)
    {
        ui->entryNameLineEdit->clear();
        ui->usernameLineEdit->clear();
        ui->passwordLineEdit->clear();
        ui->repeatedPasswordLineEdit->clear();
        ui->notesTextEdit->clear();
        if (db->size() > 0)
        {
            ui->entryTableWidget->selectRow(0);
            row = 0;
        }
    }
    ui->entryNameLineEdit->setText(db->name(row));
    ui->usernameLineEdit->setText(db->username(row));
    ui->passwordLineEdit->setText(db->password(row));
    ui->repeatedPasswordLineEdit->setText(db->password(row));
    ui->notesTextEdit->setPlainText(db->notes(row));
    ui->entryTableWidget->selectRow(row);
}

int PassMan::selectedItem() // Return currently selected item in entry list
{
    if (ui->entryTableWidget->selectedItems().length() > 0) return ui->entryTableWidget->selectedItems().at(0)->row();
    return -1;
}

void PassMan::on_actionClose_Database_triggered() { close(); }  // Close database, saving if needed

bool PassMan::close()  // Handle possible database closing
{
    if (!isOpen) return true;
    if (!isSaved)
    {
        switch (confirmClose(CLOSE_TITLE, CLOSE_QUESTION))
        {
            case QMessageBox::Save:
                save(false);
                if (!isSaved) return false; // Don't close if user failed to save after trying to
                break;
            case QMessageBox::Cancel:
                return false;
            case QMessageBox::Discard:
                break;
        }
    }
    isOpen = false;
    db->clear();    // Don't leave any sensitive data
    auth->clean();
    ui->passwordLineEdit->setStyleSheet(LINEEDIT_WHITE_BG);
    ui->repeatedPasswordLineEdit->setStyleSheet(LINEEDIT_WHITE_BG);
    passMismatch = false;
    updateActions();
    updateListInfo(-1);
    return true;
}

void PassMan::on_actionQuit_triggered() // Quit the program
{
    if (close())
    {
        tester->hide(); // Don't leave other windows hanging around!
        auth->hide();
        about->hide();
        gen->hide();
        strength->hide();
        help->hide();
        QApplication::quit();
    }
}

void PassMan::on_actionDelete_Entry_triggered() // Remove entry from the database
{
    int row = selectedItem();
    isSaved = false;
    ui->entryNameLineEdit->blockSignals(true);
    ui->usernameLineEdit->blockSignals(true);
    ui->passwordLineEdit->blockSignals(true);
    ui->repeatedPasswordLineEdit->blockSignals(true);
    ui->notesTextEdit->blockSignals(true);
    db->remove(row);
    updateListInfo(-1);
    updateDisplayInfo(-1);
    updateActions();
    ui->usernameLineEdit->blockSignals(false);
    ui->passwordLineEdit->blockSignals(false);
    ui->repeatedPasswordLineEdit->blockSignals(false);
    ui->notesTextEdit->blockSignals(false);
    ui->entryNameLineEdit->blockSignals(false);
}

void PassMan::on_actionAdd_Entry_triggered()    // Add new entry to the database
{
    int row = db->size();
    isSaved = false;
    ui->entryNameLineEdit->blockSignals(true);
    ui->usernameLineEdit->blockSignals(true);
    ui->passwordLineEdit->blockSignals(true);
    ui->repeatedPasswordLineEdit->blockSignals(true);
    ui->notesTextEdit->blockSignals(true);
    db->addNew(); // Insert new item and update GUI
    updateListInfo(-1);
    updateDisplayInfo(row);
    updateActions();
    ui->entryNameLineEdit->blockSignals(false);
    ui->usernameLineEdit->blockSignals(false);
    ui->passwordLineEdit->blockSignals(false);
    ui->repeatedPasswordLineEdit->blockSignals(false);
    ui->notesTextEdit->blockSignals(false);
}

void PassMan::on_actionAbout_triggered(){ about->show(); }  // Display about info

void PassMan::on_actionHow_to_Use_triggered() { help->show(); } // Display how to use info

void PassMan::on_actionYubiKey_Tester_triggered() { tester->show(); }  // Open YubiKey Tester

void PassMan::on_actionPassword_Generator_triggered() { gen->show(); } // Open password generator

void PassMan::passGenDone() // Update after password generation done
{
    QString pass = gen->getPassword();
    if (isOpen)
    {
        ui->passwordLineEdit->setText(pass);
        ui->repeatedPasswordLineEdit->setText(pass);
        int strength = round(StrengthCalculator::naiveEntropyBits(pass));
        if (ui->passwordStrengthBar->maximum() < strength) ui->passwordStrengthBar->setMaximum(strength);
        ui->passwordStrengthBar->setValue(round(StrengthCalculator::naiveEntropyBits(pass)));
        updatePasswords();
    }
}

void PassMan::closeEvent(QCloseEvent *event)    // Override the window close event to ensure cleanup is completed!
{
    if (close())
    {
        tester->hide(); // Don't leave other windows hanging around!
        auth->hide();
        about->hide();
        gen->hide();
        strength->hide();
        event->accept();
        QApplication::quit();
    }
    event->ignore();    // User decided not to close
}

void PassMan::on_generatePasswordButton_clicked() { gen->show(); }

void PassMan::on_actionCopy_Entry_Username_triggered()
{
    QClipboard* cb = QApplication::clipboard();
    cb->setText(ui->usernameLineEdit->text());
}

void PassMan::on_actionCopy_Entry_Password_triggered()
{
    QClipboard* cb = QApplication::clipboard();
    cb->setText(ui->passwordLineEdit->text());
}

void PassMan::on_actionPassword_Strength_Calculator_triggered()
{
    strength->clear();
    strength->show();
}

void PassMan::on_actionAbout_Qt_triggered() { QMessageBox::aboutQt(ui->passManCentralWidget); } // Show Qt info window

void PassMan::on_actionAuto_Type_Entry_triggered()  // Perform auto-type
{
    QString command("xdotool -");
    QString tabCommand = "key Tab";
    QString returnCommand = "key Return";

    this->setWindowState(Qt::WindowMinimized);
    QProcess* proc = new QProcess();    // Send username
    proc->start(command, QIODevice::ReadWrite);
    proc->write(ui->usernameLineEdit->text().prepend("type ").toUtf8());
    proc->closeWriteChannel();
    proc->waitForFinished(-1);
    proc->close();

    proc->start(command, QIODevice::ReadWrite);
    proc->write(tabCommand.toUtf8());  // Send tab key
    proc->closeWriteChannel();
    proc->waitForFinished(-1);
    proc->close();

    proc->start(command, QIODevice::ReadWrite);
    proc->write(ui->passwordLineEdit->text().prepend("type ").toUtf8());  // Send password
    proc->closeWriteChannel();
    proc->waitForFinished(-1);
    proc->close();

    proc->start(command, QIODevice::ReadWrite);
    proc->write(returnCommand.toUtf8());    // Send return key
    proc->closeWriteChannel();
    proc->waitForFinished(-1);
    proc->close();
    delete proc;
}
