/*
 * Description: Implementation of the StrengthCalculator class.
 *              Provides metrics on the strength of password strings.
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#include "strengthcalculator.h"
#include "ui_strengthcalculator.h"

StrengthCalculator::StrengthCalculator(QWidget *parent) : QWidget(parent), ui(new Ui::StrengthCalculator)
{
    ui->setupUi(this);
}

StrengthCalculator::~StrengthCalculator()
{
    delete ui;
}

void StrengthCalculator::clear()
{
    ui->passwordLineEdit->clear();
    ui->strengthProgressBar->setValue(0);
}

double StrengthCalculator::naiveEntropyBits(const QString &pw)  // Calculate raw bits of entropy (assuming password made with uniform probability distribution)
{
    bool hasLower, hasUpper, hasNumeral, hasOther;
    hasLower = hasUpper = hasNumeral = hasOther = false;
    int len = pw.length();
    for (int i = 0; i < len; i++)
    {
        QChar ch = pw.at(i);
        if (ch.isLower()) hasLower = true;
        if (ch.isUpper()) hasUpper = true;
        if (ch.isDigit()) hasNumeral = true;
        if (ch.isSymbol() || ch.isSpace()) hasOther = true;
    }
    int possibleSymbols = 0;
    if (hasLower) possibleSymbols += NUM_LOWER;
    if (hasUpper) possibleSymbols += NUM_UPPER;
    if (hasNumeral) possibleSymbols += NUM_NUMERAL;
    if (hasOther) possibleSymbols += NUM_OTHER;
    return (possibleSymbols > 0) ? ((double) len) * log2((double) possibleSymbols) : 0.0;   // Avoid undefined log_2(0)w
}

double StrengthCalculator::shannonEntropyBits(const QString &pw)    // Calculate bits of entropy (using Shannon's user-selection statistical estimates)
{
    return 0.0;
}

void StrengthCalculator::on_passwordLineEdit_textChanged(const QString &arg1)
{
    int strength = round(naiveEntropyBits(arg1));
    if (ui->strengthProgressBar->maximum() < strength) ui->strengthProgressBar->setMaximum(strength);
    ui->strengthProgressBar->setValue(strength);
}

void StrengthCalculator::on_revealPasswordCheckbox_clicked(bool checked)
{
    if (checked) ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
    else ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
}
