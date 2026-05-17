/////////////////////////////////////////////////////////////////////////////
// Name:        appsettings.cpp
// Purpose:     Connection/login/export data
// Author:      Jan Buchholz
// Created:     2026-05-01
// Changed:     2026-05-08
/////////////////////////////////////////////////////////////////////////////

#include "appsettings.h"

AppSettings::AppPreferences AppSettings::m_settings{};

AppSettings::AppPreferences AppSettings::getSettings() {
    return m_settings;
}

void AppSettings::setSettings(AppSettings::AppPreferences s) {
    m_settings = s;
}

