/*
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#include "about.h"
#include "ui_about.h"

const QString About::LINK_TEXT = "<a href=\"https://adamcoffee.net/\">adamcoffee.net</a>";
const QString About::USING_1_TEXT = "<a href=\"https://developers.yubico.com/yubikey-personalization/\">- yubikey-personalization 1.18.0</a>";
const QString About::USING_2_TEXT = "<a href=\"https://www.cryptopp.com/\">- crypto++ 5.6.5</a>";
const QString About::USING_3_TEXT = "<a href=\"http://doc.qt.io/qt-5/\">- Qt 5.8.0</a>";
const QString About::USING_4_TEXT = "<a href=\"http://www.semicomplete.com/projects/xdotool\">- xdotool 3.20150503.1</a>";

About::About(const QString& version, QWidget *parent) : QWidget(parent), ui(new Ui::About)
{
    license = new License();
    ui->setupUi(this);
    ui->titleLabel->setText(version);
    ui->linkLabel->setText(LINK_TEXT);  // Config the website link to open browser
    ui->linkLabel->setTextFormat(Qt::RichText);
    ui->linkLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->linkLabel->setOpenExternalLinks(true);
    ui->usingLabel1->setText(USING_1_TEXT); // Repeat for software links
    ui->usingLabel1->setTextFormat(Qt::RichText);
    ui->usingLabel1->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->usingLabel1->setOpenExternalLinks(true);
    ui->usingLabel2->setText(USING_2_TEXT);
    ui->usingLabel2->setTextFormat(Qt::RichText);
    ui->usingLabel2->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->usingLabel2->setOpenExternalLinks(true);
    ui->usingLabel3->setText(USING_3_TEXT);
    ui->usingLabel3->setTextFormat(Qt::RichText);
    ui->usingLabel3->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->usingLabel3->setOpenExternalLinks(true);
    ui->usingLabel4->setText(USING_4_TEXT);
    ui->usingLabel4->setTextFormat(Qt::RichText);
    ui->usingLabel4->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->usingLabel4->setOpenExternalLinks(true);
}

About::~About()
{
    delete ui;
    delete license;
}

void About::on_pushButton_clicked() { license->show(); }    // Show license
