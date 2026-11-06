/////////////////////////////////////////////////////////////////////////////
// Name:        splashdialog.h
// Purpose:     Overlay dialog with Emby logo
// Author:      Jan Buchholz
// Created:     2025-04-23
// Changed:     2026-11-06
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QDialog>
#include <QMainWindow>

namespace Ui {
class SplashDialog;
}

class SplashDialog : public QDialog {
    Q_OBJECT

public:
    explicit SplashDialog(QMainWindow *parent = nullptr);
    ~SplashDialog();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::SplashDialog *ui;
    QPixmap m_logoPixmap;

};
