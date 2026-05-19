/////////////////////////////////////////////////////////////////////////////
// Name:       sqlreader.h
// Purpose:    Access local Emby dump DB
// Author:     Jan Buchholz
// Created:    2026-05-16
// Changed:    2026-05-19
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include "globals.h"
#include "sqlreturntypes.h"

class SqlReader : public QObject {
    Q_OBJECT

public:
    explicit SqlReader(QObject* parent = nullptr);
    ErrorCode openAndCheckDB(QString fileName);
    void closeDBConnection();
    EmbyCollectionResult loadCollections();
    ByteArrayResult loadImage(const QString& parentId);
    MoviesDataImp loadMovies(const QString& collectionId);
    SeriesDataImp loadSeries(const QString& collectionId);
    HomeVideosDataImp loadHomeVideos(const QString& collectionId);
    MusicVideosDataImp loadMusicVideos(const QString& collectionId);
    MusicDataImp loadMusic(const QString& collectionId);

private:
    QString m_dbFile;

    VectorStringResult loadGenres(const QSqlDatabase& db,
                                  const QString& collectionId,
                                  const QString& parentId);
    VectorStringResult loadStudios(const QSqlDatabase& db,
                                   const QString& collectionId,
                                   const QString& parentId);
    VectorStringResult loadPeople(const QSqlDatabase& db,
                                  const QString& collectionId,
                                  const QString& parentId,
                                  const QString& personType);
    FolderDataResult loadFolders(const QSqlDatabase& db,
                                 const QString& collectionId);
    QString lastDBError(const QSqlQuery& q);
};


