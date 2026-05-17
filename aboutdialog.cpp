/////////////////////////////////////////////////////////////////////////////
// Name:        aboutdialog.cpp
// Purpose:     Create About dialog
// Author:      Jan Buchholz
// Created:     2026-05-04
// Changed:     2026-05-10
/////////////////////////////////////////////////////////////////////////////

#include <QPushButton>
#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AboutDialog) {
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Close"));
    setFixedSize(this->geometry().width(),this->geometry().height());
    ui->lbl_text1->setText(TxtAbout);
    setStyleSheet(
        "QDialog {"
        "  border: 1px solid #666;"
        "  border-radius: 6px;"
        "  background: qlineargradient("
        "      x1:0, y1:0, x2:0, y2:1,"
        "      stop:0 #ffffff,"
        "      stop:1 #e6e6e6"
        "  );"
        "}"
    );
}

AboutDialog::~AboutDialog() {
    delete ui;
}
