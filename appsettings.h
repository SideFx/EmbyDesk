/////////////////////////////////////////////////////////////////////////////
// Name:        appsettings.h
// Purpose:     Connection/login/export data
// Author:      Jan Buchholz
// Created:     2026-05-01
// Changed:     2026-05-08
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QString>
#include <QFont>

class AppSettings {
public:
    AppSettings() = delete;

    struct AppPreferences {
        bool secure;
        QString host;
        QString port;
        QString userName;
        QString userPw;
        int max_actors;
        int max_directors;
        int max_studios;
        int max_genres;
        bool limit_actors;
        bool limit_directors;
        bool limit_studios;
        bool limit_genres;
    };

    static AppPreferences getSettings();
    static void setSettings(AppPreferences s);

private:
    static AppPreferences m_settings;
};


