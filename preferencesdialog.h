/////////////////////////////////////////////////////////////////////////////
// Name:        preferencesdialog.h
// Purpose:     Popup to enter Emby server connection/login settings
// Author:      Jan Buchholz
// Created:     2026-04-23
// Changed:     2026-05-06
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QDialog>
#include "appsettings.h"

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog {
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog();

    void setSettings(AppSettings::AppPreferences settings);
    AppSettings::AppPreferences getSettings();

private:
    Ui::PreferencesDialog *ui;
};

