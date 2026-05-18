/////////////////////////////////////////////////////////////////////////////
// Name:       dumpworker.h
// Purpose:    Perform dump of Emby collections
// Author:     Jan Buchholz
// Created:    2026-05-09
// Changed:    2026-05-18
/////////////////////////////////////////////////////////////////////////////

#include "dumpworker.h"
#include <QThread>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include "apicall.h"
#include "conversions.hpp"

DumpWorker::DumpWorker(const QString& directory, const QString& dbName, QObject* parent)
    : QObject(parent), m_directory(directory), m_dbName(dbName) {}

void DumpWorker::run() {
    // ---Reading collections for user---
    emit log(tr("Fetching Emby Collections ..."));
    UserViewsResult userViews = APICall::userGetViews();
    if (userViews.code != 0) {
        emit log(toQString(userViews.message));
        return;
    }
    // ---Log collection details---
    for (auto& v : userViews.views) {
        QString s = tr("Found Collection '%1', Type: %2, ID: %3")
        .arg(v.name)
            .arg(v.collectionType)
            .arg(v.id);
        emit log(s);
    }
    // ---Creating database---
    emit log(tr("Creating sqlite Database ..."));
    bool b = createDb();
    if (b) {
        for (auto& v : userViews.views) {
            // ---Insert collections---
            QSqlQuery qc(m_db);
            qc.prepare(
                "INSERT OR REPLACE INTO collection (id, name, type) "
                "VALUES (:id, :name, :type)"
            );
            qc.bindValue(":id", toQString(v.id));
            qc.bindValue(":name", toQString(v.name));
            qc.bindValue(":type", toQString(v.collectionType));
            if (!qc.exec()) {
                emit log(tr("ERROR inserting into table 'collection': ") + lastDBError(qc));
                b = false;
                break;
            }
        }
        QSqlQuery qm(m_db);
        qm.prepare("INSERT OR REPLACE INTO meta (key, value) VALUES (:key, :value)");
        // ---Schema Version---
        qm.bindValue(":key", "schema_version");
        qm.bindValue(":value", "1");
        qm.exec();
        // ---App---
        qm.bindValue(":key", "app_name");
        qm.bindValue(":value", APP_NAME);
        qm.exec();        
        // ---App Version (UI)---
        qm.bindValue(":key", "app_version");
        qm.bindValue(":value", APP_VERSION);
        qm.exec();
        // ---Backend API Version---
        qm.bindValue(":key", "api_version");
        qm.bindValue(":value", APICall::getBackendAPIVersion());
        qm.exec();
        // ---Emby Server Url---
        qm.bindValue(":key", "emby_server");
        qm.bindValue(":value", APICall::getEmbyBaseUrl());
        qm.exec();
        // ---Dump Date---
        qm.bindValue(":key", "dump_date");
        qm.bindValue(":value", QDateTime::currentDateTime().toString(Qt::ISODate));
        qm.exec();
        if (b) {
            // ---Process collections---
            for (auto& v : userViews.views) {
                emit log(QString(tr("Processing Collection '%1' ...")).arg(v.name));
                if (!processCollection(v)) {
                    b = false;
                    break;
                }
            }
        }
    };
    emit log(tr("Closing database ..."));
    m_db.close();
    QSqlDatabase::removeDatabase(DEF_SQLITE_CONNECTION);
    if (b) emit log(tr("Process finished."));
    else emit log(tr("Process aborted with error."));
    emit finished();
}

bool DumpWorker::createDb() {
    QDir dir(m_directory);
    QString fullPath = dir.filePath(m_dbName);
    // Verbindung anlegen
    m_db = QSqlDatabase::addDatabase("QSQLITE", DEF_SQLITE_CONNECTION);
    m_db.setDatabaseName(fullPath);
    if (!m_db.open()) {
        emit log(tr("ERROR: Cannot open SQLite database! ") + m_db.lastError().text());
        return false;
    }
    QSqlQuery q(m_db);
    // Table: collection
    if (!q.exec(
            "CREATE TABLE collection ("
            "id TEXT PRIMARY KEY,"
            "name TEXT,"
            "type TEXT"
            ")"
            )) {
        emit log(tr("ERROR: Cannot create table 'collection' ") + lastDBError(q));
        return false;
    }
    emit log(tr("Created table 'collection'"));
    // Table: meta
    if (!q.exec(
            "CREATE TABLE meta ("
            "key TEXT PRIMARY KEY,"
            "value TEXT"
            ")"
            )) {
        emit log(tr("ERROR: Cannot create table 'meta' ") + lastDBError(q));
        return false;
    }
    emit log(tr("Created table 'meta'"));
    // Table: genre
    if (!q.exec(
            "CREATE TABLE genre ("
            "collection_id TEXT,"
            "parent_id TEXT,"
            "genre TEXT,"
            "PRIMARY KEY (collection_id, parent_id, genre)"
            ")"
            )) {
        emit log(tr("ERROR: Cannot create table 'genre' ") + lastDBError(q));
        return false;
    }
    emit log(tr("Created table 'genre'"));
    // Index
    if (!q.exec(
            "CREATE INDEX idx_genre_parent ON genre(parent_id)"
            )) {
        emit log(tr("ERROR: Cannot create index 'idx_genre_parent' ") + lastDBError(q));
        return false;
    }
    emit log(tr("Created index 'idx_genre_parent'"));
    // Table: studio
    if (!q.exec(
            "CREATE TABLE studio ("
            "collection_id TEXT,"
            "parent_id TEXT,"
            "studio TEXT,"
            "PRIMARY KEY (collection_id, parent_id, studio)"
            ")"
            )) {
        emit log(tr("ERROR: Cannot create table 'studio' ") + lastDBError(q));
        return false;
    }
    emit log(tr("Created table 'studio'"));
    // Index
    if (!q.exec(
            "CREATE INDEX idx_studio_parent ON studio(parent_id)"
            )) {
        emit log(tr("ERROR: Cannot create index 'idx_studio_parent' ") + lastDBError(q));
        return false;
    }
    emit log(tr("Created index 'idx_studio_parent'"));
    // Table: people
    if (!q.exec(
            "CREATE TABLE people ("
            "collection_id TEXT,"
            "parent_id TEXT,"
            "name TEXT,"
            "role TEXT,"
            "sort_order INTEGER,"
            "PRIMARY KEY (collection_id, parent_id, name, role)"
            ")"
            )) {
        emit log(tr("ERROR: Cannot create table 'people' ") + lastDBError(q));
        return false;
    }
    emit log(tr("Created table 'people'"));
    // Index
    if (!q.exec(
            "CREATE INDEX idx_people_parent ON people(parent_id)"
            )) {
        emit log(tr("ERROR: Cannot create index 'idx_people_parent' ") + lastDBError(q));
        return false;
    }
    emit log(tr("Created index 'idx_people_parent'"));
    // Table: folder
    if (!q.exec(
            "CREATE TABLE folder ("
            "collection_id TEXT,"
            "folder_id TEXT,"
            "name TEXT,"
            "PRIMARY KEY (collection_id, folder_id)"
            ")"
            )) {
        emit log(tr("ERROR: Cannot create table 'folder' ") + lastDBError(q));
        return false;
    }
    emit log(tr("Created table 'folder'"));
    // Index
    if (!q.exec(
            "CREATE INDEX idx_folder_id ON folder(folder_id)"
            )) {
        emit log(tr("ERROR: Cannot create index 'idx_folder_id' ") + lastDBError(q));
        return false;
    }
    emit log(tr("Created index 'idx_folder_id'"));
    // Table: image
    if (!q.exec(
            "CREATE TABLE image ("
            "parent_id TEXT PRIMARY KEY,"
            "image BLOB"
            ")"
            )) {
        emit log(tr("ERROR: Cannot create table 'image' ") + lastDBError(q));
        return false;
    }
    emit log(tr("Created table 'image'"));
    // Index
    if (!q.exec(
            "CREATE INDEX idx_image_parent ON image(parent_id)"
            )) {
        emit log(tr("ERROR: Cannot create index 'idx_image_parent' ") + lastDBError(q));
        return false;
    }
    emit log(tr("Created index 'idx_image_parent'"));
    if (!createMovieTable()) return false;
    if (!createSeriesTables()) return false;
    if (!createHomeVideoTable()) return false;
    if (!createMusicVideoTable()) return false;
    if (!createMusicTables()) return false;
    return true;
}

bool DumpWorker::processCollection(UserView& view) {
    QString const prepareInsertPeople =
        "INSERT OR IGNORE INTO people (collection_id, parent_id, name, role, sort_order) "
        "VALUES (:collection_id, :parent_id, :name, :role, :sort_order)";
    QString const prepareInsertStudio =
        "INSERT OR IGNORE INTO studio (collection_id, parent_id, studio) "
        "VALUES (:collection_id, :parent_id, :studio)";
    QString const prepareInsertGenre =
        "INSERT OR IGNORE INTO genre (collection_id, parent_id, genre) "
        "VALUES (:collection_id, :parent_id, :genre)";
    QString const prepareInsertFolder =
        "INSERT OR IGNORE INTO folder (collection_id, folder_id, name) "
        "VALUES (:collection_id, :folder_id, :name)";
    QString const prepareInsertImage =
        "INSERT OR REPLACE INTO image (parent_id, image) "
        "VALUES (:parent_id, :image)";
    int32_t sort_order;
    QString sql;
    QSqlQuery q(m_db);
    // ---Process Movies---
    if (view.collectionType == CollectionMovies) {
        m_movies = APICall::userGetMovieData(view.id);
        if (m_movies.code != 0) {
            emit log(toQString(m_movies.message));
            return false;
        }
        sql =
            "INSERT OR REPLACE INTO movie ("
            "collection_id, id, name, original_title, production_year, runtime, "
            "overview, container, audio_codec, video_codec, width, height, bitrate, "
            "file_size, file_name, imdb_id, added_at, folder_id"
            ") VALUES ("
            ":collection_id, :id, :name, :original_title, :production_year, :runtime, "
            ":overview, :container, :audio_codec, :video_codec, :width, :height, :bitrate, "
            ":file_size, :file_name, :imdb_id, :added_at, :folder_id"
            ")";
        if (!q.prepare(sql)) {
            emit log(tr("ERROR: Failed to prepare INSERT for table 'movie' ") + lastDBError(q));
            return false;
        }
        for (auto& m : m_movies.movies.tMovieData) {
            q.bindValue(":collection_id", toQString(view.id));
            q.bindValue(":id", toQString(m.movieId));
            q.bindValue(":name", toQString(m.name));
            q.bindValue(":original_title", toQString(m.originalTitle));
            q.bindValue(":production_year", m.productionYear);
            q.bindValue(":runtime", m.runtime);
            q.bindValue(":overview", toQString(m.overview));
            q.bindValue(":container", toQString(m.container));
            q.bindValue(":audio_codec", toQString(m.audioCodec));
            q.bindValue(":video_codec", toQString(m.videoCodec));
            q.bindValue(":width", m.width);
            q.bindValue(":height", m.height);
            q.bindValue(":bitrate", m.bitrate);
            q.bindValue(":file_size", m.fileSize);
            q.bindValue(":file_name", toQString(m.fileName));
            q.bindValue(":imdb_id", toQString(m.imDbId));
            q.bindValue(":added_at", m.addedAt);
            q.bindValue(":folder_id", toQString(m.folderId));
            if (!q.exec()) {
                emit log(tr("ERROR inserting into table 'movie' ") + lastDBError(q));
                return false;
            }
            QSqlQuery qp(m_db);
            qp.prepare(prepareInsertPeople);
            sort_order = 0;
            for (auto& a : m.actors) {
                qp.bindValue(":collection_id", toQString(view.id));
                qp.bindValue(":parent_id", toQString(m.movieId));
                qp.bindValue(":name", toQString(a));
                qp.bindValue(":role", toQString(ActorPersonType));
                qp.bindValue(":sort_order", ++sort_order);
                if (!qp.exec()) {
                    emit log(tr("ERROR inserting into table 'person' ") + lastDBError(qp));
                }
            }
            sort_order = 0;
            for (auto& d : m.directors) {
                qp.bindValue(":collection_id", toQString(view.id));
                qp.bindValue(":parent_id", toQString(m.movieId));
                qp.bindValue(":name", toQString(d));
                qp.bindValue(":role", toQString(DirectorPersonType));
                qp.bindValue(":sort_order", ++sort_order);
                if (!qp.exec()) {
                    emit log(tr("ERROR inserting into table 'person' ") + lastDBError(qp));
                }
            }
            QSqlQuery qs(m_db);
            qs.prepare(prepareInsertStudio);
            for (auto& s : m.studios) {
                qs.bindValue(":collection_id", toQString(view.id));
                qs.bindValue(":parent_id", toQString(m.movieId));
                qs.bindValue(":studio", toQString(s));
                if (!qs.exec()) {
                    emit log(tr("ERROR inserting into table 'studio' ") + lastDBError(qs));
                }
            }
            QSqlQuery qg(m_db);
            qg.prepare(prepareInsertGenre);
            for (auto& g : m.genres) {
                qg.bindValue(":collection_id", toQString(view.id));
                qg.bindValue(":parent_id", toQString(m.movieId));
                qg.bindValue(":genre", toQString(g));
                if (!qg.exec()) {
                    emit log(tr("ERROR inserting into table 'genre' ") + lastDBError(qg));
                }
            }
        }
        QSqlQuery qf(m_db);
        qf.prepare(prepareInsertFolder);
        for (auto& f : m_movies.movies.tFolderData) {
            qf.bindValue(":collection_id", toQString(view.id));
            qf.bindValue(":folder_id", toQString(f.folderId));
            qf.bindValue(":name", toQString(f.name));
            if (!qf.exec()) {
                emit log(tr("ERROR inserting into table 'folder' ") + lastDBError(qf));
                return false;
            }
        }
        emit log("Fetching images for movies, this might take a while ...");
        for (auto& m : m_movies.movies.tMovieData) {
            if (m.primaryImageTag == "") continue;
            ItemImageImp img = APICall::getPrimaryImageForItem(m.primaryImageId, m.primaryImageTag, true);
            if (img.code == 0) {
                QByteArray imgData = QByteArray::fromBase64(QByteArray::fromStdString(img.imageData));
                if (imgData.isEmpty()) {
                    continue;
                }
                QSqlQuery qi(m_db);
                qi.prepare(prepareInsertImage);
                qi.bindValue(":parent_id", toQString(m.movieId));
                qi.bindValue(":image", imgData);
                if (!qi.exec()) {
                    emit log(tr("ERROR inserting into table 'image' ") + lastDBError(qi));
                }
            } else {
                emit log(toQString(img.message));
            }
        }
    }
    // ---Process Series---
    if (view.collectionType == CollectionSeries) {
        m_series = APICall::userGetSeriesData(view.id);
        if (m_series.code != 0) {
            emit log(toQString(m_series.message));
            return false;
        }
        sql =
            "INSERT OR REPLACE INTO series ("
            "collection_id, id, name, original_title, production_year, overview, added_at, imdb_id) "
            "VALUES (:collection_id, :id, :name, :original_title, :production_year, "
            ":overview, :added_at, :imdb_id)";
        if (!q.prepare(sql)) {
            emit log(tr("ERROR: Failed to prepare INSERT for table 'series' ") + lastDBError(q));
            return false;
        }
        for (auto& s : m_series.series.tSeriesData) {
            q.bindValue(":collection_id", toQString(view.id));
            q.bindValue(":id", toQString(s.seriesId));
            q.bindValue(":name", toQString(s.name));
            q.bindValue(":original_title", toQString(s.originalTitle));
            q.bindValue(":production_year", s.productionYear);
            q.bindValue(":overview", toQString(s.overview));
            q.bindValue(":added_at", s.addedAt);
            q.bindValue(":imdb_id", toQString(s.imDbId));
            if (!q.exec()) {
                emit log(tr("ERROR inserting into table 'series' ") + lastDBError(q));
                return false;
            }
            QSqlQuery qp(m_db);
            qp.prepare(prepareInsertPeople);
            sort_order = 0;
            for (auto& a : s.actors) {
                qp.bindValue(":collection_id", toQString(view.id));
                qp.bindValue(":parent_id", toQString(s.seriesId));
                qp.bindValue(":name", toQString(a));
                qp.bindValue(":role", toQString(ActorPersonType));
                qp.bindValue(":sort_order", ++sort_order);
                if (!qp.exec()) {
                    emit log(tr("ERROR inserting into table 'person' ") + lastDBError(qp));
                }
            }
            sort_order = 0;
            for (auto& d : s.directors) {
                qp.bindValue(":collection_id", toQString(view.id));
                qp.bindValue(":parent_id", toQString(s.seriesId));
                qp.bindValue(":name", toQString(d));
                qp.bindValue(":role", toQString(DirectorPersonType));
                qp.bindValue(":sort_order", ++sort_order);
                if (!qp.exec()) {
                    emit log(tr("ERROR inserting into table 'person' ") + lastDBError(qp));
                }
            }
            QSqlQuery qs(m_db);
            qs.prepare(prepareInsertStudio);
            for (auto& st : s.studios) {
                qs.bindValue(":collection_id", toQString(view.id));
                qs.bindValue(":parent_id", toQString(s.seriesId));
                qs.bindValue(":studio", toQString(st));
                if (!qs.exec()) {
                    emit log(tr("ERROR inserting into table 'studio' ") + lastDBError(qs));
                }
            }
            QSqlQuery qg(m_db);
            qg.prepare(prepareInsertGenre);
            for (auto& g : s.genres) {
                qg.bindValue(":collection_id", toQString(view.id));
                qg.bindValue(":parent_id", toQString(s.seriesId));
                qg.bindValue(":genre", toQString(g));
                if (!qg.exec()) {
                    emit log(tr("ERROR inserting into table 'genre' ") + lastDBError(qg));
                }
            }
        }
        emit log("Fetching images for series, this might take a while ...");
        for (auto& s : m_series.series.tSeriesData) {
            if (s.primaryImageTag == "") continue;
            ItemImageImp img = APICall::getPrimaryImageForItem(s.primaryImageId, s.primaryImageTag, true);
            if (img.code == 0) {
                QByteArray imgData = QByteArray::fromBase64(QByteArray::fromStdString(img.imageData));
                if (imgData.isEmpty()) {
                    continue;
                }
                QSqlQuery qi(m_db);
                qi.prepare(prepareInsertImage);
                qi.bindValue(":parent_id", toQString(s.seriesId));
                qi.bindValue(":image", imgData);
                if (!qi.exec()) {
                    emit log(tr("ERROR inserting into table 'image' ") + lastDBError(qi));
                }
            } else {
                emit log(toQString(img.message));
            }
        }
        sql =
            "INSERT OR REPLACE INTO season ("
            "collection_id, series_id, id, name, production_year, runtime, added_at, sort_index) "
            "VALUES (:collection_id, :series_id, :id, :name, :production_year, :runtime, :added_at, "
            ":sort_index)";
        if (!q.prepare(sql)) {
            emit log(tr("ERROR: Failed to prepare INSERT for table 'season' ") + lastDBError(q));
            return false;
        }
        for (auto& s : m_series.series.tSeasonData) {
            q.bindValue(":collection_id", toQString(view.id));
            q.bindValue(":series_id", toQString(s.seriesId));
            q.bindValue(":id", toQString(s.seasonId));
            q.bindValue(":name", toQString(s.name));
            q.bindValue(":production_year", s.productionYear);
            q.bindValue(":runtime", s.runtime);
            q.bindValue(":added_at", s.addedAt);
            q.bindValue(":sort_index", s.sortIndex);
            if (!q.exec()) {
                emit log(tr("ERROR inserting into table 'season' ") + lastDBError(q));
                return false;
            }
        }
        emit log("Fetching images for seasons, this might take a while ...");
        for (auto& s : m_series.series.tSeasonData) {
            if (s.primaryImageTag == "") continue;
            ItemImageImp img = APICall::getPrimaryImageForItem(s.primaryImageId, s.primaryImageTag, true);
            if (img.code == 0) {
                QByteArray imgData = QByteArray::fromBase64(QByteArray::fromStdString(img.imageData));
                if (imgData.isEmpty()) {
                    continue;
                }
                QSqlQuery qi(m_db);
                qi.prepare(prepareInsertImage);
                qi.bindValue(":parent_id", toQString(s.seasonId));
                qi.bindValue(":image", imgData);
                if (!qi.exec()) {
                    emit log(tr("ERROR inserting into table 'image' ") + lastDBError(qi));
                }
            } else {
                emit log(toQString(img.message));
            }
        }
        sql =
            "INSERT OR REPLACE INTO episode ("
            "collection_id, series_id, season_id, id, name, original_title, production_year, runtime, "
            "overview, container, audio_codec, video_codec, width, height, bitrate, "
            "file_size, file_name, added_at, imdb_id, sort_index"
            ") VALUES ("
            ":collection_id, :series_id, :season_id, :id, :name, :original_title, :production_year, :runtime, "
            ":overview, :container, :audio_codec, :video_codec, :width, :height, :bitrate, "
            ":file_size, :file_name, :added_at, :imdb_id, :sort_index"
            ")";
        if (!q.prepare(sql)) {
            emit log(tr("ERROR: Failed to prepare INSERT for table 'episode' ") + lastDBError(q));
            return false;
        }
        for (auto& e : m_series.series.tEpisodeData) {
            q.bindValue(":collection_id", toQString(view.id));
            q.bindValue(":series_id", toQString(e.seriesId));
            q.bindValue(":season_id", toQString(e.seasonId));
            q.bindValue(":id", toQString(e.episodeId));
            q.bindValue(":name", toQString(e.name));
            q.bindValue(":original_title", toQString(e.originalTitle));
            q.bindValue(":production_year", e.productionYear);
            q.bindValue(":runtime", e.runtime);
            q.bindValue(":overview", toQString(e.overview));
            q.bindValue(":container", toQString(e.container));
            q.bindValue(":audio_codec", toQString(e.audioCodec));
            q.bindValue(":video_codec", toQString(e.videoCodec));
            q.bindValue(":width", e.width);
            q.bindValue(":height", e.height);
            q.bindValue(":bitrate", e.bitrate);
            q.bindValue(":file_size", e.fileSize);
            q.bindValue(":file_name", toQString(e.fileName));
            q.bindValue(":added_at", e.addedAt);
            q.bindValue(":imdb_id", toQString(e.imDbId));
            q.bindValue(":sort_index", e.sortIndex);
            if (!q.exec()) {
                emit log(tr("ERROR inserting into table 'episode': ") + lastDBError(q));
                return false;
            }
            QSqlQuery qp(m_db);
            qp.prepare(prepareInsertPeople);
            sort_order = 0;
            for (auto& a : e.actors) {
                qp.bindValue(":collection_id", toQString(view.id));
                qp.bindValue(":parent_id", toQString(e.episodeId));
                qp.bindValue(":name", toQString(a));
                qp.bindValue(":role", toQString(ActorPersonType));
                qp.bindValue(":sort_order", ++sort_order);
                if (!qp.exec()) {
                    emit log(tr("ERROR inserting into table 'person' ") + lastDBError(qp));
                }
            }
            sort_order = 0;
            for (auto& d : e.directors) {
                qp.bindValue(":collection_id", toQString(view.id));
                qp.bindValue(":parent_id", toQString(e.episodeId));
                qp.bindValue(":name", toQString(d));
                qp.bindValue(":role", toQString(DirectorPersonType));
                qp.bindValue(":sort_order", ++sort_order);
                if (!qp.exec()) {
                    emit log(tr("ERROR inserting into table 'person' ") + lastDBError(qp));
                }
            }
        }
        emit log("Fetching images for episodes, this might take a while ...");
        for (auto& e : m_series.series.tEpisodeData) {
            if (e.primaryImageTag == "") continue;
            ItemImageImp img = APICall::getPrimaryImageForItem(e.primaryImageId, e.primaryImageTag, true);
            if (img.code == 0) {
                QByteArray imgData = QByteArray::fromBase64(QByteArray::fromStdString(img.imageData));
                if (imgData.isEmpty()) {
                    continue;
                }
                QSqlQuery qi(m_db);
                qi.prepare(prepareInsertImage);
                qi.bindValue(":parent_id", toQString(e.episodeId));
                qi.bindValue(":image", imgData);
                if (!qi.exec()) {
                    emit log(tr("ERROR inserting into table 'image' ") + lastDBError(qi));
                }
            } else {
                emit log(toQString(img.message));
            }
        }
    }
    // ---Process HomeVideos---
    if (view.collectionType == CollectionHomeVideos) {
        m_homeVideos = APICall::userGetHomeVideoData(view.id);
        if (m_homeVideos.code != 0) {
            emit log(toQString(m_homeVideos.message));
            return false;
        }
        sql =
            "INSERT OR REPLACE INTO homevideo ("
            "collection_id, id, name, production_year, runtime, "
            "overview, container, audio_codec, video_codec, width, height, bitrate, "
            "file_size, file_name, added_at, folder_id"
            ") VALUES ("
            ":collection_id, :id, :name, :production_year, :runtime, "
            ":overview, :container, :audio_codec, :video_codec, :width, :height, :bitrate, "
            ":file_size, :file_name, :added_at, :folder_id"
            ")";
        if (!q.prepare(sql)) {
            emit log(tr("ERROR: Failed to prepare INSERT for table 'homevideo' ") + lastDBError(q));
            return false;
        }
        for (auto& v : m_homeVideos.homeVideos.tHomeVideoData) {
            q.bindValue(":collection_id", toQString(view.id));
            q.bindValue(":id", toQString(v.videoId));
            q.bindValue(":name", toQString(v.name));
            q.bindValue(":production_year", v.productionYear);
            q.bindValue(":runtime", v.runtime);
            q.bindValue(":overview", toQString(v.overview));
            q.bindValue(":container", toQString(v.container));
            q.bindValue(":audio_codec", toQString(v.audioCodec));
            q.bindValue(":video_codec", toQString(v.videoCodec));
            q.bindValue(":width", v.width);
            q.bindValue(":height", v.height);
            q.bindValue(":bitrate", v.bitrate);
            q.bindValue(":file_size", v.fileSize);
            q.bindValue(":file_name", toQString(v.fileName));
            q.bindValue(":added_at", v.addedAt);
            q.bindValue(":folder_id", toQString(v.folderId));
            if (!q.exec()) {
                emit log(tr("ERROR inserting into table 'homevideo' ") + lastDBError(q));
                return false;
            }
            QSqlQuery qg(m_db);
            qg.prepare(prepareInsertGenre);
            for (auto& g : v.genres) {
                qg.bindValue(":collection_id", toQString(view.id));
                qg.bindValue(":parent_id", toQString(v.videoId));
                qg.bindValue(":genre", toQString(g));
                if (!qg.exec()) {
                    emit log(tr("ERROR inserting into table 'genre' ") + lastDBError(qg));
                }
            }
        }
        QSqlQuery qf(m_db);
        qf.prepare(prepareInsertFolder);
        for (auto& f : m_homeVideos.homeVideos.tFolderData) {
            qf.bindValue(":collection_id", toQString(view.id));
            qf.bindValue(":folder_id", toQString(f.folderId));
            qf.bindValue(":name", toQString(f.name));
            if (!qf.exec()) {
                emit log(tr("ERROR inserting into table 'folder' ") + lastDBError(qf));
                return false;
            }
        }
        emit log("Fetching images for home videos, this might take a while ...");
        for (auto& v : m_homeVideos.homeVideos.tHomeVideoData) {
            if (v.primaryImageTag == "") continue;
            ItemImageImp img = APICall::getPrimaryImageForItem(v.primaryImageId, v.primaryImageTag, true);
            if (img.code == 0) {
                QByteArray imgData = QByteArray::fromBase64(QByteArray::fromStdString(img.imageData));
                if (imgData.isEmpty()) {
                    continue;
                }
                QSqlQuery qi(m_db);
                qi.prepare(prepareInsertImage);
                qi.bindValue(":parent_id", toQString(v.videoId));
                qi.bindValue(":image", imgData);
                if (!qi.exec()) {
                    emit log(tr("ERROR inserting into table 'image' ") + lastDBError(qi));
                }
            } else {
                emit log(toQString(img.message));
            }
        }
    }
    // ---Process MusicVideos---
    if (view.collectionType == CollectionMusicVideos) {
        m_musicVideos = APICall::userGetMusicVideoData(view.id);
        if (m_musicVideos.code != 0) {
            emit log(toQString(m_musicVideos.message));
            return false;
        }
        sql =
            "INSERT OR REPLACE INTO musicvideo ("
            "collection_id, id, name, production_year, runtime, "
            "overview, container, audio_codec, video_codec, width, height, bitrate, "
            "file_size, file_name, added_at, folder_id"
            ") VALUES ("
            ":collection_id, :id, :name, :production_year, :runtime, "
            ":overview, :container, :audio_codec, :video_codec, :width, :height, :bitrate, "
            ":file_size, :file_name, :added_at, :folder_id"
            ")";
        if (!q.prepare(sql)) {
            emit log(tr("ERROR: Failed to prepare INSERT for table 'musicvideo' ") + lastDBError(q));
            return false;
        }
        for (auto& v : m_musicVideos.musicVideos.tMusicVideoData) {
            q.bindValue(":collection_id", toQString(view.id));
            q.bindValue(":id", toQString(v.videoId));
            q.bindValue(":name", toQString(v.name));
            q.bindValue(":production_year", v.productionYear);
            q.bindValue(":runtime", v.runtime);
            q.bindValue(":overview", toQString(v.overview));
            q.bindValue(":container", toQString(v.container));
            q.bindValue(":audio_codec", toQString(v.audioCodec));
            q.bindValue(":video_codec", toQString(v.videoCodec));
            q.bindValue(":width", v.width);
            q.bindValue(":height", v.height);
            q.bindValue(":bitrate", v.bitrate);
            q.bindValue(":file_size", v.fileSize);
            q.bindValue(":file_name", toQString(v.fileName));
            q.bindValue(":added_at", v.addedAt);
            q.bindValue(":folder_id", toQString(v.folderId));
            if (!q.exec()) {
                emit log(tr("ERROR inserting into table 'musicvideo' ") + lastDBError(q));
                return false;
            }
            QSqlQuery qp(m_db);
            qp.prepare(prepareInsertPeople);
            sort_order = 0;
            for (auto& a : v.artists) {
                qp.bindValue(":collection_id", toQString(view.id));
                qp.bindValue(":parent_id", toQString(v.videoId));
                qp.bindValue(":name", toQString(a));
                qp.bindValue(":role", toQString(ArtistPersonType));
                qp.bindValue(":sort_order", ++sort_order);
                if (!qp.exec()) {
                    emit log(tr("ERROR inserting into table 'person' ") + lastDBError(qp));
                }
            }
            QSqlQuery qg(m_db);
            qg.prepare(prepareInsertGenre);
            for (auto& g : v.genres) {
                qg.bindValue(":collection_id", toQString(view.id));
                qg.bindValue(":parent_id", toQString(v.videoId));
                qg.bindValue(":genre", toQString(g));
                if (!qg.exec()) {
                    emit log(tr("ERROR inserting into table 'genre' ") + lastDBError(qg));
                }
            }
        }
        QSqlQuery qf(m_db);
        qf.prepare(prepareInsertFolder);
        for (auto& f : m_musicVideos.musicVideos.tFolderData) {
            qf.bindValue(":collection_id", toQString(view.id));
            qf.bindValue(":folder_id", toQString(f.folderId));
            qf.bindValue(":name", toQString(f.name));
            if (!qf.exec()) {
                emit log(tr("ERROR inserting into table 'folder' ") + lastDBError(qf));
                return false;
            }
        }
        emit log("Fetching images for music videos, this might take a while ...");
        for (auto& v : m_musicVideos.musicVideos.tMusicVideoData) {
            if (v.primaryImageTag == "") continue;
            ItemImageImp img = APICall::getPrimaryImageForItem(v.primaryImageId, v.primaryImageTag, true);
            if (img.code == 0) {
                QByteArray imgData = QByteArray::fromBase64(QByteArray::fromStdString(img.imageData));
                if (imgData.isEmpty()) {
                    continue;
                }
                QSqlQuery qi(m_db);
                qi.prepare(prepareInsertImage);
                qi.bindValue(":parent_id", toQString(v.videoId));
                qi.bindValue(":image", imgData);
                if (!qi.exec()) {
                    emit log(tr("ERROR inserting into table 'image' ") + lastDBError(qi));
                }
            } else {
                emit log(toQString(img.message));
            }
        }
    }
    // ---Process Music---
    if (view.collectionType == CollectionMusic) {
        m_music = APICall::userGetMusicData(view.id);
        if (m_music.code != 0) {
            emit log(toQString(m_music.message));
            return false;
        }
        sql =
            "INSERT OR REPLACE INTO album ("
            "collection_id, id, name, production_year, album_artist, runtime, "
            "added_at, musicbrainz_id"
            ") VALUES ("
            ":collection_id, :id, :name, :production_year, :album_artist, :runtime, "
            ":added_at, :musicbrainz_id"
            ")";
        if (!q.prepare(sql)) {
            emit log(tr("ERROR: Failed to prepare INSERT for table 'album' ") + lastDBError(q));
            return false;
        }
        for (auto& a : m_music.music.tAlbumData) {
            q.bindValue(":collection_id", toQString(view.id));
            q.bindValue(":id", toQString(a.albumId));
            q.bindValue(":name", toQString(a.name));
            q.bindValue(":production_year", a.productionYear);
            q.bindValue(":runtime", a.runtime);
            q.bindValue(":album_artist", toQString(a.albumArtist));
            q.bindValue(":runtime", a.runtime);
            q.bindValue(":added_at", a.addedAt);
            q.bindValue(":musicbrainz_id", toQString(a.musicBrainzId));
            if (!q.exec()) {
                emit log(tr("ERROR inserting into table 'album' ") + lastDBError(q));
                return false;
            }
            QSqlQuery qp(m_db);
            qp.prepare(prepareInsertPeople);
            sort_order = 0;
            for (auto& ar : a.artists) {
                qp.bindValue(":collection_id", toQString(view.id));
                qp.bindValue(":parent_id", toQString(a.albumId));
                qp.bindValue(":name", toQString(ar));
                qp.bindValue(":role", toQString(ArtistPersonType));
                qp.bindValue(":sort_order", ++sort_order);
                if (!qp.exec()) {
                    emit log(tr("ERROR inserting into table 'person' ") + lastDBError(qp));
                }
            }
            QSqlQuery qg(m_db);
            qg.prepare(prepareInsertGenre);
            for (auto& g : a.genres) {
                qg.bindValue(":collection_id", toQString(view.id));
                qg.bindValue(":parent_id", toQString(a.albumId));
                qg.bindValue(":genre", toQString(g));
                if (!qg.exec()) {
                    emit log(tr("ERROR inserting into table 'genre' ") + lastDBError(qg));
                }
            }
        }
        emit log("Fetching images for albums, this might take a while ...");
        for (auto& a : m_music.music.tAlbumData) {
            if (a.primaryImageTag == "") continue;
            ItemImageImp img = APICall::getPrimaryImageForItem(a.primaryImageId, a.primaryImageTag, true);
            if (img.code == 0) {
                QByteArray imgData = QByteArray::fromBase64(QByteArray::fromStdString(img.imageData));
                if (imgData.isEmpty()) {
                    continue;
                }
                QSqlQuery qi(m_db);
                qi.prepare(prepareInsertImage);
                qi.bindValue(":parent_id", toQString(a.albumId));
                qi.bindValue(":image", imgData);
                if (!qi.exec()) {
                    emit log(tr("ERROR inserting into table 'image' ") + lastDBError(qi));
                }
            } else {
                emit log(toQString(img.message));
            }
        }
        sql =
            "INSERT OR REPLACE INTO audio ("
            "collection_id, album_id, id, name, production_year, disc_number, track_number, album, "
            "album_artist, runtime, container, audio_codec, bitrate, "
            "file_size, file_name, added_at, media_type"
            ") VALUES ("
            ":collection_id, :album_id, :id, :name, :production_year, :disc_number, :track_number, :album, "
              ":album_artist, :runtime, :container, :audio_codec, :bitrate, "
              ":file_size, :file_name, :added_at, :media_type"
            ")";
        if (!q.prepare(sql)) {
            emit log(tr("ERROR: Failed to prepare INSERT for table 'audio' ") + lastDBError(q));
            return false;
        }
        for (auto& a : m_music.music.tAudioData) {
            q.bindValue(":collection_id", toQString(view.id));
            q.bindValue(":album_id", toQString(a.albumId));
            q.bindValue(":id", toQString(a.audioId));
            q.bindValue(":name", toQString(a.name));
            q.bindValue(":disc_number", a.discNumber);
            q.bindValue(":track_number", a.trackNumber);
            q.bindValue(":production_year", a.productionYear);
            q.bindValue(":album", toQString(a.album));
            q.bindValue(":album_artist", toQString(a.albumArtist));
            q.bindValue(":runtime", a.runtime);
            q.bindValue(":container", toQString(a.container));
            q.bindValue(":audio_codec", toQString(a.audioCodec));
            q.bindValue(":bitrate", a.bitrate);
            q.bindValue(":file_size", a.fileSize);
            q.bindValue(":file_name", toQString(a.fileName));
            q.bindValue(":added_at", a.addedAt);
            q.bindValue(":media_type", toQString(a.mediaType));
            if (!q.exec()) {
                emit log(tr("ERROR inserting into table 'audio' ") + lastDBError(q));
                return false;
            }
            QSqlQuery qp(m_db);
            qp.prepare(prepareInsertPeople);
            sort_order = 0;
            for (auto& ar : a.artists) {
                qp.bindValue(":collection_id", toQString(view.id));
                qp.bindValue(":parent_id", toQString(a.audioId));
                qp.bindValue(":name", toQString(ar));
                qp.bindValue(":role", toQString(ArtistPersonType));
                qp.bindValue(":sort_order", ++sort_order);
                if (!qp.exec()) {
                    emit log(tr("ERROR inserting into table 'person' ") + lastDBError(qp));
                }
            }
            QSqlQuery qg(m_db);
            qg.prepare(prepareInsertGenre);
            for (auto& g : a.genres) {
                qg.bindValue(":collection_id", toQString(view.id));
                qg.bindValue(":parent_id", toQString(a.audioId));
                qg.bindValue(":genre", toQString(g));
                if (!qg.exec()) {
                    emit log(tr("ERROR inserting into table 'genre' ") + lastDBError(qg));
                }
            }
        }
        emit log("Fetching images for audio tracks, this might take a while ...");
        for (auto& a : m_music.music.tAudioData) {
            if (a.primaryImageTag == "") continue;
            ItemImageImp img = APICall::getPrimaryImageForItem(a.primaryImageId, a.primaryImageTag, true);
            if (img.code == 0) {
                QByteArray imgData = QByteArray::fromBase64(QByteArray::fromStdString(img.imageData));
                if (imgData.isEmpty()) {
                    continue;
                }
                QSqlQuery qi(m_db);
                qi.prepare(prepareInsertImage);
                qi.bindValue(":parent_id", toQString(a.audioId));
                qi.bindValue(":image", imgData);
                if (!qi.exec()) {
                    emit log(tr("ERROR inserting into table 'image' ") + lastDBError(qi));
                }
            } else {
                emit log(toQString(img.message));
            }
        }
    }
    return true;
}

bool DumpWorker::createMovieTable() {
    QSqlQuery q(m_db);
    QString sql =
        "CREATE TABLE movie"
        " ("
        "collection_id TEXT,"
        "id TEXT,"
        "name TEXT,"
        "original_title TEXT,"
        "production_year INTEGER,"
        "runtime INTEGER,"
        "overview TEXT,"
        "container TEXT,"
        "audio_codec TEXT,"
        "video_codec TEXT,"
        "width INTEGER,"
        "height INTEGER,"
        "bitrate INTEGER,"
        "file_size INTEGER,"
        "file_name TEXT,"
        "imdb_id TEXT,"
        "added_at INTEGER,"
        "folder_id TEXT,"
        "PRIMARY KEY (collection_id, id)"
        ")";
    if (!q.exec(sql)) {
        emit log(tr("ERROR: Cannot create 'movie' table ") + lastDBError(q));
        return false;
    }
    emit log(QString(tr("Created table 'movie'")));
    return true;
}

bool DumpWorker::createSeriesTables() {
    QString sql;
    QSqlQuery q(m_db);
    // ---Series table---
    sql = "CREATE TABLE series"
          " ("
          "collection_id TEXT,"
          "id TEXT,"
          "name TEXT,"
          "original_title TEXT,"
          "production_year INTEGER,"
          "overview TEXT,"
          "added_at INTEGER,"
          "imdb_id TEXT,"
          "PRIMARY KEY (collection_id, id)"
          ")";
    if (!q.exec(sql)) {
        emit log(tr("ERROR: Cannot create 'series' table ") + lastDBError(q));
        return false;
    }
    emit log(QString(tr("Created table 'series'")));
    // ---Season table---
    sql = "CREATE TABLE season"
          " ("
          "collection_id TEXT,"
          "series_id TEXT,"
          "id TEXT,"
          "name TEXT,"
          "production_year INTEGER,"
          "runtime INTEGER,"
          "added_at INTEGER,"
          "sort_index INTEGER,"
          "PRIMARY KEY (collection_id, series_id, id)"
          ")";
    if (!q.exec(sql)) {
        emit log(tr("ERROR: Cannot create 'season' table ") + lastDBError(q));
        return false;
    }
    emit log(QString(tr("Created table 'season'")));
    // ---Episode table---
    sql = "CREATE TABLE episode"
          " ("
          "collection_id TEXT,"
          "series_id TEXT,"
          "season_id TEXT,"
          "id TEXT,"
          "name TEXT,"
          "original_title TEXT,"
          "production_year INTEGER,"
          "runtime INTEGER,"
          "overview TEXT,"
          "container TEXT,"
          "audio_codec TEXT,"
          "video_codec TEXT,"
          "width INTEGER,"
          "height INTEGER,"
          "bitrate INTEGER,"
          "file_size INTEGER,"
          "file_name TEXT,"
          "added_at INTEGER,"
          "imdb_id TEXT,"
          "sort_index INTEGER,"
          "PRIMARY KEY (collection_id, series_id, season_id, id)"
          ")";
    if (!q.exec(sql)) {
        emit log(tr("ERROR: Cannot create 'episode' table ") + lastDBError(q));
        return false;
    }
    emit log(QString(tr("Created table 'episode'")));
    return true;
}

bool DumpWorker::createHomeVideoTable() {
    QSqlQuery q(m_db);
    // ---HomeVideo table---
    QString sql = "CREATE TABLE homevideo"
                  " ("
                  "collection_id TEXT,"
                  "id TEXT,"
                  "name TEXT,"
                  "production_year INTEGER,"
                  "runtime INTEGER,"
                  "overview TEXT,"
                  "container TEXT,"
                  "audio_codec TEXT,"
                  "video_codec TEXT,"
                  "width INTEGER,"
                  "height INTEGER,"
                  "bitrate INTEGER,"
                  "file_size INTEGER,"
                  "file_name TEXT,"
                  "added_at INTEGER,"
                  "folder_id TEXT,"
                  "PRIMARY KEY (collection_id, id)"
                  ")";
    if (!q.exec(sql)) {
        emit log(tr("ERROR: Cannot create 'homevideo' table ") + lastDBError(q));
        return false;
    }
    emit log(QString(tr("Created table 'homevideo'")));
    return true;
}

bool DumpWorker::createMusicVideoTable() {
    QSqlQuery q(m_db);
    // ---MusicVideo table---
    QString sql = "CREATE TABLE musicvideo"
                  " ("
                  "collection_id TEXT,"
                  "id TEXT,"
                  "name TEXT,"
                  "production_year INTEGER,"
                  "runtime INTEGER,"
                  "overview TEXT,"
                  "container TEXT,"
                  "audio_codec TEXT,"
                  "video_codec TEXT,"
                  "width INTEGER,"
                  "height INTEGER,"
                  "bitrate INTEGER,"
                  "file_size INTEGER,"
                  "file_name TEXT,"
                  "added_at INTEGER,"
                  "folder_id TEXT,"
                  "PRIMARY KEY (collection_id, id)"
                  ")";
    if (!q.exec(sql)) {
        emit log(tr("ERROR: Cannot create 'musicvideo' table ") + lastDBError(q));
        return false;
    }
    emit log(QString(tr("Created table 'musicvideo'")));
    return true;
}

bool DumpWorker::createMusicTables() {
    QString sql;
    QSqlQuery q(m_db);
    // ---Album table---
    sql = "CREATE TABLE album"
          " ("
          "collection_id TEXT,"
          "id TEXT,"
          "name TEXT,"
          "production_year INTEGER,"
          "album_artist TEXT,"
          "runtime INTEGER,"
          "added_at INTEGER,"
          "musicbrainz_id TEXT,"
          "PRIMARY KEY (collection_id, id)"
          ")";
    if (!q.exec(sql)) {
        emit log(tr("ERROR: Cannot create 'album' table ") + lastDBError(q));
        return false;
    }
    emit log(QString(tr("Created table 'album'")));
    // ---Audio table---
    sql = "CREATE TABLE audio"
          " ("
          "collection_id TEXT,"
          "album_id TEXT,"
          "id TEXT,"
          "name TEXT,"
          "production_year INTEGER,"
          "disc_number INTEGER,"
          "track_number INTEGER,"
          "album TEXT,"
          "album_artist TEXT,"
          "runtime INTEGER,"
          "container TEXT,"
          "audio_codec TEXT,"
          "bitrate INTEGER,"
          "file_size INTEGER,"
          "file_name TEXT,"
          "added_at INTEGER,"
          "media_type TEXT,"
          "PRIMARY KEY (collection_id, album_id, id)"
          ")";
    if (!q.exec(sql)) {
        emit log(tr("ERROR: Cannot create 'audio' table ") + lastDBError(q));
        return false;
    }
    emit log(QString(tr("Created table 'audio'")));
    return true;
}

QString DumpWorker::lastDBError(const QSqlQuery& q) {
    QString error = q.lastError().text();
    if (!error.isEmpty()) {
        return "\n" + tr("DB error: ") + error;
    }
    return {};
}

