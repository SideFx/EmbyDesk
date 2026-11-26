/////////////////////////////////////////////////////////////////////////////
// Name:        splashdialog.h
// Purpose:     Overlay dialog with Emby logo
// Author:      Jan Buchholz
// Created:     2025-04-23
// Changed:     2026-11-26
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QDialog>
#include <QMainWindow>
#include <QIcon>

namespace Ui {
class SplashDialog;
}

class SplashDialog : public QDialog {
    Q_OBJECT

public:
    explicit SplashDialog(QMainWindow *parent = nullptr);
    ~SplashDialog();

private:
    Ui::SplashDialog *ui;

};
