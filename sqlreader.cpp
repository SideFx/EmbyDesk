/////////////////////////////////////////////////////////////////////////////
// Name:       sqlreader.cpp
// Purpose:    Access local Emby dump DB
// Author:     Jan Buchholz
// Created:    2026-05-16
// Changed:    2026-05-19
/////////////////////////////////////////////////////////////////////////////

#include "sqlreader.h"
#include <QSqlQuery>
#include <QSqlError>
#include "conversions.hpp"

SqlReader::SqlReader(QObject* parent) : QObject(parent) {}

ErrorCode SqlReader::openAndCheckDB(QString fileName) {
    m_dbFile = fileName;
    closeDBConnection();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", DEF_SQLITE_CONNECTION);
    db.setDatabaseName(m_dbFile);
    if (!db.open()) return MSG_DB_OPEN_ERROR;
    QSqlQuery q(db);
    q.prepare("SELECT value FROM meta WHERE key='app_name'");
    if (!q.exec() || !q.next()) {
        db.close();
        return MSG_DB_META_ERROR;
    }
    if (q.value(0).toString() != APP_NAME) {
        db.close();
        return MSG_DB_NOT_RECOGNIZED;
    }
    return MSG_OK;
}

void SqlReader::closeDBConnection() {
    if (QSqlDatabase::contains(DEF_SQLITE_CONNECTION)) {
        QSqlDatabase::removeDatabase(DEF_SQLITE_CONNECTION);
    }
}

EmbyCollectionResult SqlReader::loadCollections() {
    EmbyCollectionResult r;
    QSqlDatabase db = QSqlDatabase::database(DEF_SQLITE_CONNECTION);
    if (!db.isOpen()) {
        r.code = MSG_DB_NOT_OPEN;
        r.message = ERROR_MESSAGES[r.code];
        return r;
    }
    QSqlQuery q(db);
    q.prepare("SELECT id, name, type FROM collection ORDER BY name COLLATE NOCASE");
    if (!q.exec()) {
        r.code = MSG_DB_QUERY_ERROR;
        r.message = ERROR_MESSAGES[r.code] + lastDBError(q);
        return r;
    }
    while (q.next()) {
        EmbyCollection c;
        c.id   = q.value(0).toString();
        c.name = q.value(1).toString();
        c.type = q.value(2).toString();
        r.collections.push_back(c);
    }
    r.code = MSG_OK;
    r.message = ERROR_MESSAGES[r.code];
    return r;
}

VectorStringResult SqlReader::loadGenres(const QSqlDatabase& db,
                                         const QString& collectionId,
                                         const QString& parentId) {
    VectorStringResult r;
    QSqlQuery q(db);
    q.prepare("SELECT genre FROM genre "
              "WHERE collection_id = ? AND parent_id = ? "
              "ORDER BY genre COLLATE NOCASE");
    q.addBindValue(collectionId);
    q.addBindValue(parentId);
    if (!q.exec()) {
        r.code = MSG_DB_QUERY_ERROR;
        r.message = toStandardString(ERROR_MESSAGES[r.code] + lastDBError(q));
        return r;
    }
    while (q.next()) {
        r.stringList.push_back(q.value(0).toString().toStdString());
    }
    r.code = MSG_OK;
    r.message = toStandardString(ERROR_MESSAGES[r.code]);
    return r;
}

VectorStringResult SqlReader::loadStudios(const QSqlDatabase& db,
                                          const QString& collectionId,
                                          const QString& parentId) {
    VectorStringResult r;
    QSqlQuery q(db);
    q.prepare("SELECT studio FROM studio "
              "WHERE collection_id = ? AND parent_id = ? "
              "ORDER BY studio COLLATE NOCASE");
    q.addBindValue(collectionId);
    q.addBindValue(parentId);
    if (!q.exec()) {
        r.code = MSG_DB_QUERY_ERROR;
        r.message = toStandardString(ERROR_MESSAGES[r.code] + lastDBError(q));
        return r;
    }
    while (q.next()) {
        r.stringList.push_back(q.value(0).toString().toStdString());
    }
    r.code = MSG_OK;
    r.message = toStandardString(ERROR_MESSAGES[r.code]);
    return r;
}

VectorStringResult SqlReader::loadPeople(const QSqlDatabase& db,
                                         const QString& collectionId,
                                         const QString& parentId,
                                         const QString& personType) {
    VectorStringResult r;
    QSqlQuery q(db);
    q.prepare("SELECT name FROM people "
              "WHERE collection_id = ? AND parent_id = ? AND role = ? "
              "ORDER BY sort_order");
    q.addBindValue(collectionId);
    q.addBindValue(parentId);
    q.addBindValue(personType);
    if (!q.exec()) {
        r.code = MSG_DB_QUERY_ERROR;
        r.message = toStandardString(ERROR_MESSAGES[r.code] + lastDBError(q));
        return r;
    }
    while (q.next()) {
        r.stringList.push_back(q.value(0).toString().toStdString());
    }
    r.code = MSG_OK;
    r.message = toStandardString(ERROR_MESSAGES[r.code]);
    return r;
}

FolderDataResult SqlReader::loadFolders(const QSqlDatabase& db,
                                        const QString& collectionId) {
    FolderDataResult r;
    QSqlQuery q(db);
    q.prepare("SELECT folder_id, name FROM folder "
              "WHERE collection_id = ?");
    q.addBindValue(collectionId);
    if (!q.exec()) {
        r.code = MSG_DB_QUERY_ERROR;
        r.message = toStandardString(ERROR_MESSAGES[r.code] + lastDBError(q));
        return r;
    }
    while (q.next()) {
        FolderDataInc f = {
            .name = q.value(1).toString().toStdString(),
            .folderId = q.value(0).toString().toStdString(),
        };
        r.tFolderData.push_back(f);
    }
    r.code = MSG_OK;
    r.message = toStandardString(ERROR_MESSAGES[r.code]);
    return r;
}

ByteArrayResult SqlReader::loadImage(const QString& parentId) {
    ByteArrayResult r;
    QSqlDatabase db = QSqlDatabase::database(DEF_SQLITE_CONNECTION);
    if (!db.isOpen()) {
        r.code = MSG_DB_NOT_OPEN;
        r.message = toStandardString(ERROR_MESSAGES[r.code]);
        return r;
    }
    QSqlQuery q(db);
    q.prepare("SELECT image FROM image WHERE parent_id = ? ");
    q.addBindValue(parentId);
    if (!q.exec()) {
        r.code = MSG_DB_QUERY_ERROR;
        r.message = toStandardString(ERROR_MESSAGES[r.code] + lastDBError(q));
        return r;
    }
    if (q.next()) {
        QByteArray arr = q.value(0).toByteArray();
        r.bytes.assign(arr.begin(), arr.end());
    }
    r.code = MSG_OK;
    r.message = toStandardString(ERROR_MESSAGES[r.code]);
    return r;
}

MoviesDataImp SqlReader::loadMovies(const QString& collectionId) {
    MoviesDataImp r;
    QSqlDatabase db = QSqlDatabase::database(DEF_SQLITE_CONNECTION);
    if (!db.isOpen()) {
        r.code = MSG_DB_NOT_OPEN;
        r.message = toStandardString(ERROR_MESSAGES[r.code]);
        return r;
    }
    QSqlQuery q(db);
    q.prepare("SELECT "
              "id, name, original_title, production_year, runtime, "
              "overview, container, audio_codec, video_codec, width, height, bitrate, "
              "file_size, file_name, imdb_id, added_at, folder_id "
              "FROM movie WHERE collection_id = ?"
              );
    q.addBindValue(collectionId);
    if (!q.exec()) {
        r.code = MSG_DB_QUERY_ERROR;
        r.message = toStandardString(ERROR_MESSAGES[r.code] + lastDBError(q));
        return r;
    }
    while (q.next()) {
        MovieDataInc m;
        int i = 0;
        m.movieId = q.value(i++).toString().toStdString();
        m.name = q.value(i++).toString().toStdString();
        m.originalTitle = q.value(i++).toString().toStdString();
        m.productionYear = q.value(i++).toInt();
        m.runtime = q.value(i++).toLongLong();
        m.overview = q.value(i++).toString().toStdString();
        m.container = q.value(i++).toString().toStdString();
        m.audioCodec = q.value(i++).toString().toStdString();
        m.videoCodec = q.value(i++).toString().toStdString();
        m.width = q.value(i++).toInt();
        m.height = q.value(i++).toInt();
        m.bitrate = q.value(i++).toInt();
        m.fileSize = q.value(i++).toLongLong();
        m.fileName = q.value(i++).toString().toStdString();
        m.imDbId = q.value(i++).toString().toStdString();
        m.addedAt = q.value(i++).toLongLong();
        m.folderId = q.value(i++).toString().toStdString();
        m.primaryImageId = m.movieId;
        m.primaryImageTag = DEF_OFFLINE;
        r.movies.tMovieData.push_back(m);
    }
    VectorStringResult vr;
    for (auto&m : r.movies.tMovieData) {
        vr = loadPeople(db, collectionId, toQString(m.movieId), toQString(ActorPersonType));
        if (vr.code == MSG_OK) {
            m.actors = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
        vr = loadPeople(db, collectionId, toQString(m.movieId), toQString(DirectorPersonType));
        if (vr.code == MSG_OK) {
            m.directors = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
        vr = loadStudios(db, collectionId, toQString(m.movieId));
        if (vr.code == MSG_OK) {
            m.studios = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
        vr = loadGenres(db, collectionId, toQString(m.movieId));
        if (vr.code == MSG_OK) {
            m.genres = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
    }
    FolderDataResult fr = loadFolders(db, collectionId);
    if (fr.code != MSG_OK) {
        r.code = fr.code;
        r.message = fr.message;
        return r;
    }
    r.movies.tFolderData = fr.tFolderData;
    r.code = MSG_OK;
    r.message = toStandardString(ERROR_MESSAGES[r.code]);
    return r;
}

SeriesDataImp SqlReader::loadSeries(const QString& collectionId) {
    SeriesDataImp r;
    QSqlDatabase db = QSqlDatabase::database(DEF_SQLITE_CONNECTION);
    if (!db.isOpen()) {
        r.code = MSG_DB_NOT_OPEN;
        r.message = toStandardString(ERROR_MESSAGES[r.code]);
        return r;
    }
    QSqlQuery q(db);
    q.prepare("SELECT "
              "id, name, original_title, production_year, overview, added_at, imdb_id "
              "FROM series WHERE collection_id = ?"
              );
    q.addBindValue(collectionId);
    if (!q.exec()) {
        r.code = MSG_DB_QUERY_ERROR;
        r.message = toStandardString(ERROR_MESSAGES[r.code] + lastDBError(q));
        return r;
    }
    while (q.next()) {
        SeriesDataInc s;
        int i = 0;
        s.seriesId = q.value(i++).toString().toStdString();
        s.name = q.value(i++).toString().toStdString();
        s.originalTitle = q.value(i++).toString().toStdString();
        s.productionYear = q.value(i++).toInt();
        s.overview = q.value(i++).toString().toStdString();
        s.addedAt = q.value(i++).toLongLong();
        s.imDbId = q.value(i++).toString().toStdString();
        s.primaryImageId = s.seriesId;
        s.primaryImageTag = DEF_OFFLINE;
        r.series.tSeriesData.push_back(s);
    }
    VectorStringResult vr;
    for (auto& s : r.series.tSeriesData) {
        vr = loadPeople(db, collectionId, toQString(s.seriesId), toQString(ActorPersonType));
        if (vr.code == MSG_OK) {
            s.actors = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
        vr = loadPeople(db, collectionId, toQString(s.seriesId), toQString(DirectorPersonType));
        if (vr.code == MSG_OK) {
            s.directors = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
        vr = loadStudios(db, collectionId, toQString(s.seriesId));
        if (vr.code == MSG_OK) {
            s.studios = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
        vr = loadGenres(db, collectionId, toQString(s.seriesId));
        if (vr.code == MSG_OK) {
            s.genres = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
    }
    q.prepare("SELECT "
              "series_id, id, name, production_year, runtime, added_at, sort_index "
              "FROM season WHERE collection_id = ?"
             );
    q.addBindValue(collectionId);
    if (!q.exec()) {
        r.code = MSG_DB_QUERY_ERROR;
        r.message = toStandardString(ERROR_MESSAGES[r.code] + lastDBError(q));
        return r;
    }
    while (q.next()) {
        SeasonDataInc s;
        int i = 0;
        s.seriesId = q.value(i++).toString().toStdString();
        s.seasonId = q.value(i++).toString().toStdString();
        s.name = q.value(i++).toString().toStdString();
        s.productionYear = q.value(i++).toInt();
        s.runtime = q.value(i++).toLongLong();
        s.addedAt = q.value(i++).toLongLong();
        s.primaryImageId = s.seasonId;
        s.primaryImageTag = DEF_OFFLINE;
        r.series.tSeasonData.push_back(s);
    }
    q.prepare("SELECT "
              "series_id, season_id, id, name, original_title, production_year, runtime, "
              "overview, container, audio_codec, video_codec, width, height, bitrate, "
              "file_size, file_name, added_at, imdb_id, sort_index "
              "FROM episode WHERE collection_id = ?"
              );
    q.addBindValue(collectionId);
    if (!q.exec()) {
        r.code = MSG_DB_QUERY_ERROR;
        r.message = toStandardString(ERROR_MESSAGES[r.code] + lastDBError(q));
        return r;
    }
    while (q.next()) {
        EpisodeDataInc e;
        int i = 0;
        e.seriesId = q.value(i++).toString().toStdString();
        e.seasonId = q.value(i++).toString().toStdString();
        e.episodeId = q.value(i++).toString().toStdString();
        e.name = q.value(i++).toString().toStdString();
        e.originalTitle = q.value(i++).toString().toStdString();
        e.productionYear = q.value(i++).toInt();
        e.runtime = q.value(i++).toInt();
        e.overview = q.value(i++).toString().toStdString();
        e.container = q.value(i++).toString().toStdString();
        e.audioCodec = q.value(i++).toString().toStdString();
        e.videoCodec = q.value(i++).toString().toStdString();
        e.width = q.value(i++).toInt();
        e.height = q.value(i++).toInt();
        e.bitrate = q.value(i++).toInt();
        e.fileSize = q.value(i++).toLongLong();
        e.fileName = q.value(i++).toString().toStdString();
        e.addedAt = q.value(i++).toLongLong();
        e.imDbId = q.value(i++).toString().toStdString();
        e.sortIndex = q.value(i++).toInt();
        e.primaryImageId = e.episodeId;
        e.primaryImageTag = DEF_OFFLINE;
        r.series.tEpisodeData.push_back(e);
    }
    for (auto& e : r.series.tEpisodeData) {
        vr = loadPeople(db, collectionId, toQString(e.episodeId), toQString(ActorPersonType));
        if (vr.code == MSG_OK) {
            e.actors = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
        vr = loadPeople(db, collectionId, toQString(e.episodeId), toQString(DirectorPersonType));
        if (vr.code == MSG_OK) {
            e.directors = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
    }
    r.code = MSG_OK;
    r.message = toStandardString(ERROR_MESSAGES[r.code]);
    return r;
}

HomeVideosDataImp SqlReader::loadHomeVideos(const QString& collectionId) {
    HomeVideosDataImp r;
    QSqlDatabase db = QSqlDatabase::database(DEF_SQLITE_CONNECTION);
    if (!db.isOpen()) {
        r.code = MSG_DB_NOT_OPEN;
        r.message = toStandardString(ERROR_MESSAGES[r.code]);
        return r;
    }
    QSqlQuery q(db);
    q.prepare("SELECT "
              "id, name, production_year, runtime, "
              "overview, container, audio_codec, video_codec, width, height, bitrate, "
              "file_size, file_name, added_at, folder_id "
              "FROM homevideo WHERE collection_id = ?"
              );
    q.addBindValue(collectionId);
    if (!q.exec()) {
        r.code = MSG_DB_QUERY_ERROR;
        r.message = toStandardString(ERROR_MESSAGES[r.code] + lastDBError(q));
        return r;
    }
    while (q.next()) {
        HomeVideoDataInc v;
        int i = 0;
        v.videoId = q.value(i++).toString().toStdString();
        v.name = q.value(i++).toString().toStdString();
        v.productionYear = q.value(i++).toInt();
        v.runtime = q.value(i++).toLongLong();
        v.overview = q.value(i++).toString().toStdString();
        v.container = q.value(i++).toString().toStdString();
        v.audioCodec = q.value(i++).toString().toStdString();
        v.videoCodec = q.value(i++).toString().toStdString();
        v.width = q.value(i++).toInt();
        v.height = q.value(i++).toInt();
        v.bitrate = q.value(i++).toInt();
        v.fileSize = q.value(i++).toLongLong();
        v.fileName = q.value(i++).toString().toStdString();
        v.addedAt = q.value(i++).toLongLong();
        v.folderId = q.value(i++).toString().toStdString();
        v.primaryImageId = v.videoId;
        v.primaryImageTag = DEF_OFFLINE;
        r.homeVideos.tHomeVideoData.push_back(v);
    }
    VectorStringResult vr;
    for (auto& v: r.homeVideos.tHomeVideoData) {
        vr = loadGenres(db, collectionId, toQString(v.videoId));
        if (vr.code == MSG_OK) {
            v.genres = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
    }
    FolderDataResult fr = loadFolders(db, collectionId);
    if (fr.code != MSG_OK) {
        r.code = fr.code;
        r.message = fr.message;
        return r;
    }
    r.homeVideos.tFolderData = fr.tFolderData;
    r.code = MSG_OK;
    r.message = toStandardString(ERROR_MESSAGES[r.code]);
    return r;
}

MusicVideosDataImp SqlReader::loadMusicVideos(const QString& collectionId) {
    MusicVideosDataImp r;
    QSqlDatabase db = QSqlDatabase::database(DEF_SQLITE_CONNECTION);
    if (!db.isOpen()) {
        r.code = MSG_DB_NOT_OPEN;
        r.message = toStandardString(ERROR_MESSAGES[r.code]);
        return r;
    }
    QSqlQuery q(db);
    q.prepare("SELECT "
              "id, name, production_year, runtime, "
              "overview, container, audio_codec, video_codec, width, height, bitrate, "
              "file_size, file_name, added_at, folder_id "
              "FROM musicvideo WHERE collection_id = ?"
              );
    q.addBindValue(collectionId);
    if (!q.exec()) {
        r.code = MSG_DB_QUERY_ERROR;
        r.message = toStandardString(ERROR_MESSAGES[r.code] + lastDBError(q));
        return r;
    }
    while (q.next()) {
        MusicVideoDataInc v;
        int i = 0;
        v.videoId = q.value(i++).toString().toStdString();
        v.name = q.value(i++).toString().toStdString();
        v.productionYear = q.value(i++).toInt();
        v.runtime = q.value(i++).toLongLong();
        v.overview = q.value(i++).toString().toStdString();
        v.container = q.value(i++).toString().toStdString();
        v.audioCodec = q.value(i++).toString().toStdString();
        v.videoCodec = q.value(i++).toString().toStdString();
        v.width = q.value(i++).toInt();
        v.height = q.value(i++).toInt();
        v.bitrate = q.value(i++).toInt();
        v.fileSize = q.value(i++).toLongLong();
        v.fileName = q.value(i++).toString().toStdString();
        v.addedAt = q.value(i++).toLongLong();
        v.folderId = q.value(i++).toString().toStdString();
        v.primaryImageId = v.videoId;
        v.primaryImageTag = DEF_OFFLINE;
        r.musicVideos.tMusicVideoData.push_back(v);
    }
    VectorStringResult vr;
    for (auto& v : r.musicVideos.tMusicVideoData) {
        vr = loadPeople(db, collectionId, toQString(v.videoId), toQString(ArtistPersonType));
        if (vr.code == MSG_OK) {
            v.artists = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
        vr = loadGenres(db, collectionId, toQString(v.videoId));
        if (vr.code == MSG_OK) {
            v.genres = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
    }
    FolderDataResult fr = loadFolders(db, collectionId);
    if (fr.code != MSG_OK) {
        r.code = fr.code;
        r.message = fr.message;
        return r;
    }
    r.musicVideos.tFolderData = fr.tFolderData;
    r.code = MSG_OK;
    r.message = toStandardString(ERROR_MESSAGES[r.code]);
    return r;
}

MusicDataImp SqlReader::loadMusic(const QString& collectionId) {
    MusicDataImp r;
    QSqlDatabase db = QSqlDatabase::database(DEF_SQLITE_CONNECTION);
    if (!db.isOpen()) {
        r.code = MSG_DB_NOT_OPEN;
        r.message = toStandardString(ERROR_MESSAGES[r.code]);
        return r;
    }
    QSqlQuery q(db);
    q.prepare("SELECT "
              "id, name, production_year, album_artist, runtime, "
              "added_at, musicbrainz_id "
              "FROM album WHERE collection_id = ?"
              );
    q.addBindValue(collectionId);
    if (!q.exec()) {
        r.code = MSG_DB_QUERY_ERROR;
        r.message = toStandardString(ERROR_MESSAGES[r.code] + lastDBError(q));
        return r;
    }
    while (q.next()) {
        AlbumDataInc a;
        int i = 0;
        a.albumId = q.value(i++).toString().toStdString();
        a.name = q.value(i++).toString().toStdString();
        a.productionYear = q.value(i++).toInt();
        a.albumArtist = q.value(i++).toString().toStdString();
        a.runtime = q.value(i++).toLongLong();
        a.addedAt = q.value(i++).toLongLong();
        a.musicBrainzId = q.value(i++).toString().toStdString();
        a.primaryImageId = a.albumId;
        a.primaryImageTag = DEF_OFFLINE;
        r.music.tAlbumData.push_back(a);
    }
    VectorStringResult vr;
    for (auto& a : r.music.tAlbumData) {
        vr = loadPeople(db, collectionId, toQString(a.albumId), toQString(ArtistPersonType));
        if (vr.code == MSG_OK) {
            a.artists = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
        vr = loadGenres(db, collectionId, toQString(a.albumId));
        if (vr.code == MSG_OK) {
            a.genres = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
    }
    q.prepare("SELECT "
              "album_id, id, name, production_year, disc_number, track_number, album, "
              "album_artist, runtime, container, audio_codec, bitrate, "
              "file_size, file_name, added_at, media_type "
              "FROM audio WHERE collection_id = ?"
             );
    q.addBindValue(collectionId);
    if (!q.exec()) {
        r.code = MSG_DB_QUERY_ERROR;
        r.message = toStandardString(ERROR_MESSAGES[r.code] + lastDBError(q));
        return r;
    }
    while (q.next()) {
        AudioDataInc a;
        int i = 0;
        a.albumId = q.value(i++).toString().toStdString();
        a.audioId = q.value(i++).toString().toStdString();
        a.name = q.value(i++).toString().toStdString();
        a.productionYear = q.value(i++).toInt();
        a.discNumber = q.value(i++).toInt();
        a.trackNumber = q.value(i++).toInt();
        a.album = q.value(i++).toString().toStdString();
        a.albumArtist = q.value(i++).toString().toStdString();
        a.runtime = q.value(i++).toLongLong();
        a.container = q.value(i++).toString().toStdString();
        a.audioCodec = q.value(i++).toString().toStdString();
        a.bitrate = q.value(i++).toInt();
        a.fileSize = q.value(i++).toLongLong();
        a.fileName = q.value(i++).toString().toStdString();
        a.addedAt = q.value(i++).toLongLong();
        a.mediaType = q.value(i++).toString().toStdString();
        a.primaryImageId = a.audioId;
        a.primaryImageTag = DEF_OFFLINE;
        r.music.tAudioData.push_back(a);
    }
    for (auto& a : r.music.tAudioData) {
        vr = loadPeople(db, collectionId, toQString(a.audioId), toQString(ArtistPersonType));
        if (vr.code == MSG_OK) {
            a.artists = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
        vr = loadGenres(db, collectionId, toQString(a.audioId));
        if (vr.code == MSG_OK) {
            a.genres = vr.stringList;
        } else {
            r.code = vr.code;
            r.message = vr.message;
            return r;
        }
    }
    r.code = MSG_OK;
    r.message = toStandardString(ERROR_MESSAGES[r.code]);
    return r;
}

QString SqlReader::lastDBError(const QSqlQuery& q) {
    QString error = q.lastError().text();
    if (!error.isEmpty()) {
        return " " + tr("Message: ") + error;
    }
    return {};
}
