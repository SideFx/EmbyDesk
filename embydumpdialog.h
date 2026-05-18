/////////////////////////////////////////////////////////////////////////////
// Name:       embydumpdialog.h
// Purpose:    Popup to prepare and view progress of dump of Emby collections
// Author:     Jan Buchholz
// Created:    2026-05-09
// Changed:    2026-05-18
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QDialog>

namespace Ui {
class EmbyDumpDialog;
}

class EmbyDumpDialog : public QDialog {
    Q_OBJECT

public:
    explicit EmbyDumpDialog(QWidget *parent = nullptr);
    ~EmbyDumpDialog();

    void setSQLiteDirectory(const QString& dir);

signals:
    void sendPathInfo(const QString& directory, const QString& dbName);

public slots:
    void logMessage(const QString& msg);

private:
    Ui::EmbyDumpDialog *ui;

    bool checkFileDoesNotExist();

    QPushButton* m_btnDump;
    QPushButton* m_btnCancel;
    QString m_directory;
    QString m_dbName;

    QString c_msg = QObject::tr("Note: The dump may take some time depending on the size of your library "
                                "and especially the number of images (covers, captured frames, etc.).");

private slots:
    void onSelect();
    void onDump();
    void onTextChanged(QString);
};


