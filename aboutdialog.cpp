/////////////////////////////////////////////////////////////////////////////
// Name:        aboutdialog.cpp
// Purpose:     Create About dialog
// Author:      Jan Buchholz
// Created:     2026-05-04
// Changed:     2026-11-06
/////////////////////////////////////////////////////////////////////////////

#include <QPushButton>
#include <QLayout>
#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AboutDialog) {
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Close"));
    ui->lbl_text1->setText(TxtAbout);
    setAutoFillBackground(true);
    adjustSize();
    setFixedSize(size());
}

AboutDialog::~AboutDialog() {
    delete ui;
}

