/////////////////////////////////////////////////////////////////////////////
// Name:        splashdialog.h
// Purpose:     Overlay dialog with Emby logo
// Author:      Jan Buchholz
// Created:     2025-04-23
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QDialog>

namespace Ui {
class SplashDialog;
}

class SplashDialog : public QDialog {
    Q_OBJECT

public:
    explicit SplashDialog(QWidget *parent = nullptr);
    ~SplashDialog();

private:
    Ui::SplashDialog *ui;
};

