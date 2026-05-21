/////////////////////////////////////////////////////////////////////////////
// Name:       sqlreader.h
// Purpose:    Access local Emby dump DB
// Author:     Jan Buchholz
// Created:    2026-05-16
// Changed:    2026-05-21
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "globals.h"
#include "sqlreturntypes.h"

class SqlReader : public QObject {
    Q_OBJECT

public:
    explicit SqlReader(QObject* parent = nullptr);
    ~SqlReader();
    ErrorCode openAndCheckDB(QString fileName);
    void closeDBConnection();
    EmbyCollectionResult loadCollections();
    ByteArrayResult loadImage(const QString& parentId);
    MoviesDataImp loadMovies(const QString& collectionId);
    SeriesDataImp loadSeries(const QString& collectionId);
    HomeVideosDataImp loadHomeVideos(const QString& collectionId);
    MusicVideosDataImp loadMusicVideos(const QString& collectionId);
    MusicDataImp loadMusic(const QString& collectionId);
    void shutdownDBConnection();

private:
    QString m_dbFile;
    QSqlDatabase m_db;

    VectorStringResult loadGenres(const QString& collectionId,
                                  const QString& parentId);
    VectorStringResult loadStudios(const QString& collectionId,
                                   const QString& parentId);
    VectorStringResult loadPeople(const QString& collectionId,
                                  const QString& parentId,
                                  const QString& personType);
    FolderDataResult loadFolders(const QString& collectionId);
    QString lastDBError(const QSqlQuery& q);
};


