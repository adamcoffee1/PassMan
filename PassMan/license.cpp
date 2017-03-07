/*
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#include "license.h"
#include "ui_license.h"

License::License(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::License)
{
    ui->setupUi(this);
}

License::~License()
{
    delete ui;
}
