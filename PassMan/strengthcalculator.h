/*
 * Description: Definition of the StrengthCalculator class.
 *              Provides metrics on the strength of password strings.
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#ifndef STRENGTHCALCULATOR_H
#define STRENGTHCALCULATOR_H

#include <QWidget>

namespace Ui
{
    class StrengthCalculator;
}

class StrengthCalculator : public QWidget
{
    Q_OBJECT

    public:
        static const int NUM_LOWER = 26; // Commonly used values
        static const int NUM_UPPER = 26;
        static const int NUM_NUMERAL = 10;
        static const int NUM_OTHER = 33;
        static const int NAIVE_HIGH_STRENGTH_ENTROPY = 128; // What is considered very strong for naive entropy calculation

        explicit StrengthCalculator(QWidget *parent = 0);
        ~StrengthCalculator();
        void clear();   // Clear out password box

        static double naiveEntropyBits(const QString& pw);  // Calculate raw bits of entropy (assuming password made with uniform probability distribution
        static double shannonEntropyBits(const QString& pw);    // Calculate bits of entropy (using Shannon's user-selection statistical estimates)

    private slots:
        void on_passwordLineEdit_textChanged(const QString &arg1);
        void on_revealPasswordCheckbox_clicked(bool checked);

    private:
        Ui::StrengthCalculator *ui;
};

#endif // STRENGTHCALCULATOR2_H
