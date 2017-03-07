/*
 * Description: Definition of the Generator class.
 *              Provides password generation with custom length and input symbols.
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include <QWidget>
#include <QClipboard>
#include <QTime>
#include <QList>
#include <QtGlobal> // For random function
#include "math.h"
#include "strengthcalculator.h"
#include <QDebug> // TESTING

namespace Ui {
class Generator;
}

class Generator : public QWidget
{
    Q_OBJECT

    public:
        explicit Generator(QWidget *parent = 0);
        ~Generator();
        QString getPassword();  // Return the newly generated password

    signals:
        void passwordGenerated();

    private slots:
        void on_revealPasswordCheckbox_clicked(bool checked);
        void on_acceptButton_clicked();
        void on_copyButton_clicked();
        void on_passwordLineEdit_textChanged(const QString &arg1);
        void on_passwordLineEdit_returnPressed();
        void on_generateButton_clicked();

        void on_lowerCheckbox_clicked(bool checked);

        void on_upperCheckbox_clicked(bool checked);

        void on_otherCheckbox_clicked(bool checked);

        void on_numeralCheckbox_clicked(bool checked);

        void on_lengthSpinBox_valueChanged(int arg1);

        void on_lengthSLider_sliderMoved(int position);

private:
        static const QString LOWER, UPPER, NUMERAL, OTHER;  // Commonly used values
        Ui::Generator *ui;
        int length;
        bool useLower, useUpper, useNumeral, useOther;

        void generate();    // Formulate a new password, given set constraints
};

#endif // GENERATOR_H
