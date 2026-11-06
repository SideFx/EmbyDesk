/////////////////////////////////////////////////////////////////////////////
// Name:        splashdialog.cpp
// Purpose:     Overlay dialog with Emby logo
// Author:      Jan Buchholz
// Created:     2026-04-23
// Changed:     2026-11-06
/////////////////////////////////////////////////////////////////////////////

#include "splashdialog.h"
#include "ui_splashdialog.h"
#include <QPainter>
#include <QPaintEvent>

SplashDialog::SplashDialog(QMainWindow *parent) : QDialog(parent), ui(new Ui::SplashDialog) {
    ui->setupUi(this);
    setAutoFillBackground(false);
    m_logoPixmap = QPixmap(":/bitmaps/assets/embylogo.png");
    adjustSize();
    setFixedSize(size());
}

SplashDialog::~SplashDialog() {
    delete ui;
}

void SplashDialog::paintEvent(QPaintEvent *event) {
    QDialog::paintEvent(event); //super->event
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    // ---center position---
    int x = (width() - m_logoPixmap.width()) / 2;
    int y = (height() - m_logoPixmap.height()) / 2;
    // paint bitmap on dialog canvas
    painter.drawPixmap(x, y, m_logoPixmap);
}

