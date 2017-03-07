/*
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#include "help.h"
#include "ui_help.h"

Help::Help(QWidget *parent) : QWidget(parent), ui(new Ui::Help)
{
    ui->setupUi(this);
}

Help::~Help()
{
    delete ui;
}
