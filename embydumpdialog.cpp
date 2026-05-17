/////////////////////////////////////////////////////////////////////////////
// Name:       embydumpdialog.cpp
// Purpose:    Popup to prepare and view progress of dump of Emby collections
// Author:     Jan Buchholz
// Created:    2026-05-09
// Changed:    2026-05-12
/////////////////////////////////////////////////////////////////////////////

#include "embydumpdialog.h"
#include "ui_embydumpdialog.h"
#include <QStandardPaths>
#include <QDateTime>
#include <QFileDialog>
#include <QLineEdit>
#include <QThread>
#include "globals.h"
#include "dumpworker.h"
#include <QMessageBox>

EmbyDumpDialog::EmbyDumpDialog(QWidget *parent) : QDialog(parent)
    , ui(new Ui::EmbyDumpDialog) {
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    setFixedSize(this->geometry().width(), this->geometry().height());
    setWindowTitle(tr("Dump Emby Collections"));
    m_btnDump = new QPushButton(this);
    m_btnDump->setEnabled(false);
    m_btnDump->setText(tr("Start Dump"));
    m_btnCancel = new QPushButton(this);
    m_btnCancel->setText(tr("Cancel"));
    ui->buttonBox->addButton(m_btnCancel, QDialogButtonBox::RejectRole);
    ui->buttonBox->addButton(m_btnDump, QDialogButtonBox::ActionRole);
    ui->edt_path->setReadOnly(true);
    ui->edt_logger->setReadOnly(true);
    QFont mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    ui->edt_logger->setFont(mono);
    connect(ui->btn_select, &QPushButton::clicked, this, &EmbyDumpDialog::onSelect);
    connect(m_btnDump, &QPushButton::clicked, this, &EmbyDumpDialog::onDump);
    connect(ui->edt_path, &QLineEdit::textChanged, this, &EmbyDumpDialog::onTextChanged);
    connect(ui->edt_filename, &QLineEdit::textChanged, this, &EmbyDumpDialog::onTextChanged);
}

EmbyDumpDialog::~EmbyDumpDialog() {
    delete ui;
}

void EmbyDumpDialog::setSQLiteDirectory(const QString& dir) {
    m_directory = (dir.isEmpty())
    ? QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
    : dir;
    const auto now = QDateTime::currentDateTime();
    QString dbName = DEF_SQLITE_PREFIX + "_" +
               now.date().toString("yyyyMMdd") +
               now.time().toString("hhmmss") +
               DEF_SQLITE_SUFFIX;
    ui->edt_path->setText(m_directory);
    ui->edt_filename->setText(dbName);
}

void EmbyDumpDialog::onSelect() {
    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Select Directory"),
        ui->edt_path->text(),
        QFileDialog::DontUseNativeDialog);
    if (!dir.isEmpty()) {
        ui->edt_path->setText(dir);
    }
}

void EmbyDumpDialog::onDump() {
    if (!checkFileDoesNotExist()) return;
    emit sendPathInfo(m_directory, m_dbName);
    // ---start Dump---
    m_btnDump->setEnabled(false);
    m_btnCancel->setEnabled(false);
    ui->edt_path->setEnabled(false);
    ui->edt_filename->setEnabled(false);
    ui->btn_select->setEnabled(false);
    // ---Worker + Thread---
    auto worker = new DumpWorker(m_directory, m_dbName);
    auto thread = new QThread(this);
    worker->moveToThread(thread);
    connect(thread, &QThread::started, worker, &DumpWorker::run);
    connect(worker, &DumpWorker::log, this, &EmbyDumpDialog::logMessage);
    connect(worker, &DumpWorker::finished, this, [=]() {
        m_btnCancel->setText(tr("Close"));
        m_btnCancel->setEnabled(true);
        thread->quit();
    });
    // ---delete worker later---
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    thread->start();
}

void EmbyDumpDialog::onTextChanged(QString) {
    m_directory = ui->edt_path->text();
    m_dbName = ui->edt_filename->text();
    m_btnDump->setEnabled(!m_directory.isEmpty() && !m_dbName.isEmpty());
}

void EmbyDumpDialog::logMessage(const QString& msg) {
    ui->edt_logger->appendPlainText(msg);
}

bool EmbyDumpDialog::checkFileDoesNotExist() {
    QDir dir(m_directory);
    QString fullPath = dir.filePath(m_dbName);
    if (QFile::exists(fullPath)) {
        QMessageBox::warning(
            this,
            tr("File exists"),
            tr("The file \"%1\" already exists.\nDump aborted.")
                .arg(fullPath)
            );
        return false;
    }
    return true;
}
