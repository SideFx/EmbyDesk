/////////////////////////////////////////////////////////////////////////////
// Name:        splashdialog.cpp
// Purpose:     Overlay dialog with Emby logo
// Author:      Jan Buchholz
// Created:     2026-04-23
// Changed:     2026-05-23
/////////////////////////////////////////////////////////////////////////////

#include "splashdialog.h"
#include "ui_splashdialog.h"

SplashDialog::SplashDialog(QMainWindow *parent) : QDialog(parent), ui(new Ui::SplashDialog) {
    ui->setupUi(this);
    setFixedSize(this->geometry().width(), this->geometry().height());
}

SplashDialog::~SplashDialog() {
    delete ui;
}

