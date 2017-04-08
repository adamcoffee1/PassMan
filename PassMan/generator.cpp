/*
 * Description: Implementation of the Generator class.
 *              Provides password generation with custom length and input symbols.
 * Author:      Adam Coffee
 *
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#include "generator.h"
#include "ui_generator.h"

const QString Generator::LOWER = "abcdefghijklmnopqrstuvwxyz";
const QString Generator::UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const QString Generator::NUMERAL = "0123456789";
const QString Generator::OTHER = "`~!@#$%^&*()-_=+[{]}\\|;:'\",<.>/? "; // Both the \ and " are escaped via an extra \ (should be 33 chars)

Generator::Generator(QWidget *parent) : QWidget(parent), ui(new Ui::Generator)
{
    ui->setupUi(this);
    ui->strengthProgressBar->setMinimum(0);
    ui->strengthProgressBar->setMaximum(StrengthCalculator::NAIVE_HIGH_STRENGTH_ENTROPY);
    useLower = useUpper = useNumeral = useOther = true;
    length = 8;
    ui->lengthSpinBox->setValue(length);
    ui->lengthSLider->setValue(length);
}

Generator::~Generator()
{
    delete ui;
}

void Generator::on_revealPasswordCheckbox_clicked(bool checked)
{
    if (checked) ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
    else ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
}

void Generator::on_acceptButton_clicked()   // Relay new password back to main program
{
    this->hide();
    emit passwordGenerated();
}

QString Generator::getPassword()    // Return the newly generated password
{
    QString pass = ui->passwordLineEdit->text();
    ui->passwordLineEdit->clear();  // Wipe new password for security
    return pass;
}

void Generator::on_copyButton_clicked() // Copy password to clipboard
{
    QClipboard* cb = QApplication::clipboard();
    cb->setText(ui->passwordLineEdit->text());
}

void Generator::on_passwordLineEdit_textChanged(const QString &arg1)
{
    int strength = StrengthCalculator::naiveEntropyBits(ui->passwordLineEdit->text());
    if (ui->strengthProgressBar->maximum() < strength) ui->strengthProgressBar->setMaximum(strength);
    ui->strengthProgressBar->setValue(strength);
    if (arg1.length() < 8)
    {
        ui->copyButton->setEnabled(false);
        ui->acceptButton->setEnabled(false);
    }
    else
    {
        ui->copyButton->setEnabled(true);
        ui->acceptButton->setEnabled(true);
    }
}

void Generator::on_passwordLineEdit_returnPressed() // Same as accept button
{
    this->hide();
    emit passwordGenerated();
}

void Generator::on_generateButton_clicked() { generate(); } // Trigger new generation

void Generator::generate()  // Formulate a new password, given set constraints
{
    QString pass;
    CryptoPP::AutoSeededRandomPool prng;    // Initialize CSPRNG
    QList<int> selectedTypes;
    bool containsAllTypes, containsLower, containsUpper, containsNumeral, containsOther;
    containsAllTypes = false;
    if (useLower) selectedTypes.append(0);
    if (useUpper) selectedTypes.append(1);
    if (useNumeral) selectedTypes.append(2);
    if (useOther) selectedTypes.append(3);
    if (selectedTypes.length() < 1) return;
    while (!containsAllTypes)
    {
        pass.clear();
        containsAllTypes = containsLower = containsUpper = containsNumeral = containsOther = false;
        for (int i = 0; i < length; i++)
        {   // Select a random type, then a random value within that type
            switch (selectedTypes.at(prng.GenerateByte() % selectedTypes.length()))
            {
                case 0:
                    pass.append(LOWER.at(prng.GenerateByte() % StrengthCalculator::NUM_LOWER));
                    break;
                case 1:
                    pass.append(UPPER.at(prng.GenerateByte() % StrengthCalculator::NUM_UPPER));
                    break;
                case 2:
                    pass.append(NUMERAL.at(prng.GenerateByte() % StrengthCalculator::NUM_NUMERAL));
                    break;
                case 3:
                    pass.append(OTHER.at(prng.GenerateByte() % StrengthCalculator::NUM_OTHER));
                    break;
            }
        }
        for (int i = 0; i < length; i++)    // Ensure all chosen types are somewhere in the password
        {
            if (LOWER.contains(pass.at(i))) containsLower = true;
            if (UPPER.contains(pass.at(i))) containsUpper = true;
            if (NUMERAL.contains(pass.at(i))) containsNumeral = true;
            if (OTHER.contains(pass.at(i))) containsOther = true;
        }
        containsAllTypes = (containsLower == useLower) && (containsUpper == useUpper) && (containsNumeral == useNumeral) && (containsOther == useOther);
    }
    ui->passwordLineEdit->setText(pass);
}

void Generator::on_lowerCheckbox_clicked(bool checked)
{
    useLower = checked;
    generate();
}

void Generator::on_upperCheckbox_clicked(bool checked)
{
    useUpper = checked;
    generate();
}
void Generator::on_otherCheckbox_clicked(bool checked)
{
    useOther = checked;
    generate();
}

void Generator::on_numeralCheckbox_clicked(bool checked)
{
    useNumeral = checked;
    generate();
}

void Generator::on_lengthSpinBox_valueChanged(int arg1)
{
    length = arg1;
    ui->lengthSLider->blockSignals(true);
    if (ui->lengthSLider->maximum() < arg1) ui->lengthSLider->setMaximum(arg1);
    ui->lengthSLider->setValue(arg1);
    ui->lengthSLider->blockSignals(false);
    generate();
}

void Generator::on_lengthSLider_sliderMoved(int position)
{
    length = position;
    ui->lengthSpinBox->blockSignals(true);
    ui->lengthSpinBox->setValue(position);
    ui->lengthSpinBox->blockSignals(false);
    generate();
}
