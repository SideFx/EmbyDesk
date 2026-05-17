/////////////////////////////////////////////////////////////////////////////
// Name:        preferencesdialog.h
// Purpose:     Popup to enter Emby server connection/login settings
// Author:      Jan Buchholz
// Created:     2026-04-23
// Changed:     2026-05-08
/////////////////////////////////////////////////////////////////////////////

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "globals.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PreferencesDialog) {
    ui->setupUi(this);
    setFixedSize(this->geometry().width(), this->geometry().height());
}

PreferencesDialog::~PreferencesDialog() {
    delete ui;
}

void PreferencesDialog::setSettings(AppSettings::AppPreferences settings) {
    ui->tabWidget->setCurrentIndex(0);
    ui->chk_https->setChecked(settings.secure);
    ui->edt_address->setText(settings.host);
    ui->edt_port->setText(settings.port);
    if (settings.port.isEmpty()) ui->edt_port->setText(DEF_EMBY_PORT);
    ui->edt_username->setText(settings.userName);
    ui->edt_password->setText(settings.userPw);
    ui->cbx_actors->setChecked(settings.limit_actors);
    ui->cbx_directors->setChecked(settings.limit_directors);
    ui->cbx_genres->setChecked(settings.limit_genres);
    ui->cbx_studios->setChecked(settings.limit_studios);
    ui->spb_maxActors->setValue(settings.max_actors);
    ui->spb_maxDirectors->setValue(settings.max_directors);
    ui->spb_maxGenres->setValue(settings.max_genres);
    ui->spb_maxStudios->setValue(settings.max_studios);
}

AppSettings::AppPreferences PreferencesDialog::getSettings() {
    AppSettings::AppPreferences settings = {
        .secure = ui->chk_https->isChecked(),
        .host = ui->edt_address->text(),
        .port = ui->edt_port->text(),
        .userName = ui->edt_username->text(),
        .userPw = ui->edt_password->text(),
        .max_actors = ui->spb_maxActors->value(),
        .max_directors = ui->spb_maxDirectors->value(),
        .max_studios = ui->spb_maxStudios->value(),
        .max_genres = ui->spb_maxGenres->value(),
        .limit_actors = ui->cbx_actors->isChecked(),
        .limit_directors = ui->cbx_directors->isChecked(),
        .limit_studios = ui->cbx_studios->isChecked(),
        .limit_genres = ui->cbx_genres->isChecked(),
    };
    return settings;
}
