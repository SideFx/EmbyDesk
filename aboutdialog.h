/////////////////////////////////////////////////////////////////////////////
// Name:        aboutdialog.h
// Purpose:     About dialog header
// Author:      Jan Buchholz
// Created:     2026-05-04
// Changed:     2026-05-10
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QDialog>
#include <QString>
#include "globals.h"

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog {
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private:
    Ui::AboutDialog *ui;
    QString TxtAbout =
        "<b>" + QString(APP_NAME) + " v" + APP_VERSION + "</b><br>" +
        " (w) 2026 Jan Buchholz<br>\u2022\u2022\u2022<br>" +
        tr("Created with Qt Community Edition v") + qVersion() + "<br>" +
        tr(" (https://www.qt.io).") +
        tr("<br>\u2022\u2022\u2022<br>Thanks to:<br>") +
        tr("Daniel Nicoletti (dantti) and Jay Two (j2Doll)<br>for \"QXlsx\"<br>") +
        tr("https://github.com/QtExcel/QXlsx");
};


