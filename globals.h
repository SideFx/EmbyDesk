/////////////////////////////////////////////////////////////////////////////
// Name:        globals.h
// Purpose:     Global constants, etc.
// Author:      Jan Buchholz
// Created:     2026-04-23
// Changed:     2026-05-19
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QString>
#include <QFont>
#include <QSize>
#include <QObject>
#include <QColor>

// ---App Settings---
#define SET_WINDOW_GEOMETRY "window-geometry"
#define SET_WINDOW_STATE "window-windowstate"
#define SET_EMBY_ADDRESS "emby-address"
#define SET_EMBY_PORT "emby-port"
#define SET_EMBY_USERNAME "emby-username"
#define SET_EMBY_PASSWORD "emby-password"
#define SET_EMBY_SECURE "emby-secure"

#define SET_MAX_ACTORS "export-actors-max"
#define SET_LIM_ACTORS "export-actors-limit"
#define SET_MAX_DIRECTORS "export-directors-max"
#define SET_LIM_DIRECTORS "export-directors-limit"
#define SET_MAX_STUDIOS "export-studios-max"
#define SET_LIM_STUDIOS "export-studios-limit"
#define SET_MAX_GENRES "export-genres-max"
#define SET_LIM_GENRES "export-genres-limit"

#define SET_FOLDER_XLSX "folder-xlsx"
#define SET_FOLDER_DB "folder-db"
#define SET_LAST_USED_DB "lastused-db"
#define APP_COMPANY "org.jan.buchholz"
#define APP_NAME "EmbyDesk"
#define APP_VERSION "1.0"
#define API_VERSION "1.1" // Go backend library

// ---Default Emby server port---
inline QString const DEF_EMBY_PORT = "8096";

// ---Message display time---
int constexpr DEF_MSG_DURATION = 8000;

// ---Toolbar---
int constexpr DEF_ICONSIZE = 16;
int constexpr DEF_TOOLBAR_HEIGHT = DEF_ICONSIZE + 10;

// ---Min. main window size---
inline QSize const DEF_WINDOW_MINSIZE = QSize(800, 600);

// ---Sheets---
int constexpr DEF_SHEET_WIDTH = 200;

// ---Size for cover images---
int constexpr DEF_IMAGE_WIDTH = 180;
int constexpr DEF_IMAGE_HEIGHT = 180;

// ---Excel export definitions---
int constexpr DEF_NO_LIMIT = 100;
inline QString const DEF_EXPORT_PREFIX = "Emby";
inline QString const DEF_EXPORT_SUFFIX = ".xlsx";
inline QString const DEF_XLSX_FILE_FILTER = QObject::tr("Excel Worksheet (*.xlsx)");
inline QColor const DEF_HEADER_COLOR(200, 240, 255);
inline QColor const DEF_SERIES_COLOR(255, 255, 220);
inline QColor const DEF_SEASON_COLOR(220, 255, 220);
inline QColor const DEF_ALBUM_COLOR(255, 255, 220);
inline QColor const DEF_DEFAULT_COLOR(245, 245, 245);

// ---Emby dump settings---
inline QString const DEF_SQLITE_CONNECTION = "emby_dump";
inline QString const DEF_SQLITE_FILE_FILTER = QObject::tr("SQLite Database (*.sqlite)");
inline QString const DEF_SQLITE_PREFIX = "EmbyCollections";
inline QString const DEF_SQLITE_SUFFIX = ".sqlite";

// ---SQL Reader settings---
inline std::string const DEF_OFFLINE = "**offline**";

enum ErrorCode {
    MSG_OK = 0,
    MSG_XLSX_WRITE_ERROR,
    MSG_DB_OPEN_ERROR,
    MSG_DB_META_ERROR,
    MSG_DB_NOT_RECOGNIZED,
    MSG_DB_NOT_OPEN,
    MSG_DB_QUERY_ERROR,
};

inline const QStringList ERROR_MESSAGES = {
    QObject::tr("OK"),
    QObject::tr("Error creating Excel file!"),
    QObject::tr("Cannot open offline database!"),
    QObject::tr("Cannot read 'meta' table."),
    QObject::tr("Database not recognized as Emby dump."),
    QObject::tr("Database is not open."),
    QObject::tr("Database SQL error."),
};

