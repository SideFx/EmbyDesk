/////////////////////////////////////////////////////////////////////////////
// Name:        splashdialog.cpp
// Purpose:     Overlay dialog with Emby logo
// Author:      Jan Buchholz
// Created:     2026-04-23
// Changed:     2026-11-26
/////////////////////////////////////////////////////////////////////////////

#include "splashdialog.h"
#include "ui_splashdialog.h"
#include <QPainter>
#include <QPaintEvent>

SplashDialog::SplashDialog(QMainWindow *parent) : QDialog(parent), ui(new Ui::SplashDialog) {
    ui->setupUi(this);
    setAutoFillBackground(true);
    adjustSize();
    setFixedSize(size());
}

SplashDialog::~SplashDialog() {
    delete ui;
}
