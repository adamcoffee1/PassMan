/*
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#ifndef ABOUT_H
#define ABOUT_H

#include <QWidget>
#include <QString>
#include "license.h"


namespace Ui
{
    class About;
}

class About : public QWidget
{
    Q_OBJECT

    public:
        explicit About(const QString& version, QWidget *parent = 0);
        ~About();

    private slots:
        void on_pushButton_clicked();

    private:
        static const QString LINK_TEXT, USING_1_TEXT, USING_2_TEXT, USING_3_TEXT, USING_4_TEXT; // Commonly used value
        License* license;
        Ui::About *ui;
};

#endif // ABOUT_H
