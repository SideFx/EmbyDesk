/////////////////////////////////////////////////////////////////////////////
// Name:       dumpworker.h
// Purpose:    Perform dump of Emby collections
// Author:     Jan Buchholz
// Created:    2026-05-09
// Changed:    2026-05-12
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include "jbparser.hpp"

class DumpWorker : public QObject {
    Q_OBJECT

public:
    explicit DumpWorker(const QString& directory,
                        const QString& dbName,
                        QObject* parent = nullptr);

public slots:
    void run();

signals:
    void log(const QString& msg);
    void finished();

private:
    bool createDb();
    bool processCollection(UserView& view);
    bool createMovieTable();
    bool createSeriesTables();
    bool createHomeVideoTable();
    bool createMusicVideoTable();
    bool createMusicTables();

    QString m_directory;
    QString m_dbName;
    QSqlDatabase m_db;

    MoviesDataImp m_movies;
    SeriesDataImp m_series;
    HomeVideosDataImp m_homeVideos;
    MusicVideosDataImp m_musicVideos;
    MusicDataImp m_music;

};


