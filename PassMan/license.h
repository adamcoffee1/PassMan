/*
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#ifndef LICENSE_H
#define LICENSE_H

#include <QWidget>

namespace Ui {
class License;
}

class License : public QWidget
{
    Q_OBJECT

public:
    explicit License(QWidget *parent = 0);
    ~License();

private:
    Ui::License *ui;
};

#endif // LICENSE_H
