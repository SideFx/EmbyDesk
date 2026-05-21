/////////////////////////////////////////////////////////////////////////////
// Name:        excelexport.cpp
// Purpose:     Export collection as Excel worksheet
// Author:      Jan Buchholz
// Created:     2026-05-07
// Changed:     2026-05-21
/////////////////////////////////////////////////////////////////////////////

#include "excelexport.h"
#include "appsettings.h"
#include "conversions.hpp"
#include "lookupkeys.h"
#include "globals.h"
#include "models/headertexts.h"
#include "models/movietreemodel.h"
#include "models/seriestreemodel.h"
#include "models/homevideotreemodel.h"
#include "models/musicvideotreemodel.h"
#include "models/musictreemodel.h"
#include "xlsxdocument.h"
#include "xlsxworkbook.h"
#include "xlsxformat.h"

ExcelExport::ExcelExport() {
    AppSettings::AppPreferences settings = AppSettings::getSettings();
    // ---Export item limits---
    int max_cast = (settings.limit_actors) ? settings.max_actors : DEF_NO_LIMIT;
    int max_directors = (settings.limit_directors) ? settings.max_directors : DEF_NO_LIMIT;
    int max_studios = (settings.limit_studios) ? settings.max_studios : DEF_NO_LIMIT;
    int max_genres = (settings.limit_genres) ? settings.max_genres : DEF_NO_LIMIT;
    // ---FieldMaps---
    movieFields = {
        {LookUpKeys::title, [](auto& d){ return toQString(d.name); }},
        {LookUpKeys::originaltitle, [](auto& d){ return toQString(d.originalTitle); }},
        {LookUpKeys::year, [](auto& d){ return int32ToYear(d.productionYear); }},
        {LookUpKeys::runtime, [](auto& d){ return int64ToRuntimeMinutes(d.runtime); }},
        {LookUpKeys::resolution, [](auto& d){ return resolutionValue(d.width, d.height); }},
        {LookUpKeys::cast, [max_cast](auto& d){ return joinList(d.actors, max_cast); }},
        {LookUpKeys::directors, [max_directors](auto& d){ return joinList(d.directors, max_directors); }},
        {LookUpKeys::genres, [max_genres](auto& d){ return joinList(d.genres, max_genres); }},
        {LookUpKeys::studios, [max_studios](auto& d){ return joinList(d.studios, max_studios); }},
        {LookUpKeys::container, [](auto& d){ return toQString(d.container); }},
        {LookUpKeys::videocodec, [](auto& d){ return toQString(d.videoCodec); }},
        {LookUpKeys::audiocodec, [](auto& d){ return toQString(d.audioCodec); }},
        {LookUpKeys::bitrate, [](auto& d){ return int32ToBitrate(d.bitrate); }},
        {LookUpKeys::imdbid, [](auto& d){ return toQString(d.imDbId); }},
        {LookUpKeys::filesize, [](auto& d){ return int64ToFileSize(d.fileSize); }},
        {LookUpKeys::filename, [](auto& d){ return toQString(d.fileName); }},
        {LookUpKeys::addedat, [](auto& d){ return int64ToDateISO(d.addedAt); }},
    };
    seriesFields = {
        {LookUpKeys::series, [](auto& d){ return toQString(d.name); }},
        {LookUpKeys::year, [](auto& d){ return int32ToYear(d.productionYear); }},
        {LookUpKeys::cast, [max_cast](auto& d){ return joinList(d.actors, max_cast); }},
        {LookUpKeys::directors, [max_directors](auto& d){ return joinList(d.directors, max_directors); }},
        {LookUpKeys::genres, [max_genres](auto& d){ return joinList(d.genres, max_genres); }},
        {LookUpKeys::studios, [max_studios](auto& d){ return joinList(d.studios, max_studios); }},
        {LookUpKeys::imdbid, [](auto& d){ return toQString(d.imDbId); }},
        {LookUpKeys::addedat, [](auto& d){ return int64ToDateISO(d.addedAt); }},
    };
    seasonFields = {
        {LookUpKeys::season, [](auto& d){ return toQString(d.name); }},
        {LookUpKeys::year, [](auto& d){ return int32ToYear(d.productionYear); }},
        {LookUpKeys::addedat, [](auto& d){ return int64ToDateISO(d.addedAt); }},
    };
    episodeFields = {
        {LookUpKeys::episode, [](auto& d){ return toQString(d.name); }},
        {LookUpKeys::year, [](auto& d){ return int32ToYear(d.productionYear); }},
        {LookUpKeys::runtime, [](auto& d){ return int64ToRuntimeMinutes(d.runtime); }},
        {LookUpKeys::resolution, [](auto& d){ return resolutionValue(d.width, d.height); }},
        {LookUpKeys::cast, [max_cast](auto& d){ return joinList(d.actors, max_cast); }},
        {LookUpKeys::directors, [max_directors](auto& d){ return joinList(d.directors, max_directors); }},
        {LookUpKeys::container, [](auto& d){ return toQString(d.container); }},
        {LookUpKeys::videocodec, [](auto& d){ return toQString(d.videoCodec); }},
        {LookUpKeys::audiocodec, [](auto& d){ return toQString(d.audioCodec); }},
        {LookUpKeys::bitrate, [](auto& d){ return int32ToBitrate(d.bitrate); }},
        {LookUpKeys::imdbid, [](auto& d){ return toQString(d.imDbId); }},
        {LookUpKeys::filesize, [](auto& d){ return int64ToFileSize(d.fileSize); }},
        {LookUpKeys::filename, [](auto& d){ return toQString(d.fileName); }},
        {LookUpKeys::addedat, [](auto& d){ return int64ToDateISO(d.addedAt); }},
    };
    homeVideoFields = {
        {LookUpKeys::title, [](auto& d){ return toQString(d.name); }},
        {LookUpKeys::year, [](auto& d){ return int32ToYear(d.productionYear); }},
        {LookUpKeys::runtime, [](auto& d){ return int64ToRuntimeMinutes(d.runtime); }},
        {LookUpKeys::resolution, [](auto& d){ return resolutionValue(d.width, d.height); }},
        {LookUpKeys::people, [max_cast](auto& d){ return joinList(d.people, max_cast); }},
        {LookUpKeys::genres, [max_genres](auto& d){ return joinList(d.genres, max_genres); }},
        {LookUpKeys::container, [](auto& d){ return toQString(d.container); }},
        {LookUpKeys::videocodec, [](auto& d){ return toQString(d.videoCodec); }},
        {LookUpKeys::audiocodec, [](auto& d){ return toQString(d.audioCodec); }},
        {LookUpKeys::bitrate, [](auto& d){ return int32ToBitrate(d.bitrate); }},
        {LookUpKeys::filesize, [](auto& d){ return int64ToFileSize(d.fileSize); }},
        {LookUpKeys::filename, [](auto& d){ return toQString(d.fileName); }},
        {LookUpKeys::addedat, [](auto& d){ return int64ToDateISO(d.addedAt); }},
    };
    musicVideoFields = {
        {LookUpKeys::title, [](auto& d){ return toQString(d.name); }},
        {LookUpKeys::year, [](auto& d){ return int32ToYear(d.productionYear); }},
        {LookUpKeys::runtime, [](auto& d){ return int64ToRuntimeMinutes(d.runtime); }},
        {LookUpKeys::resolution, [](auto& d){ return resolutionValue(d.width, d.height); }},
        {LookUpKeys::artists, [max_cast](auto& d){ return joinList(d.artists, max_cast); }},
        {LookUpKeys::genres, [max_genres](auto& d){ return joinList(d.genres, max_genres); }},
        {LookUpKeys::container, [](auto& d){ return toQString(d.container); }},
        {LookUpKeys::videocodec, [](auto& d){ return toQString(d.videoCodec); }},
        {LookUpKeys::audiocodec, [](auto& d){ return toQString(d.audioCodec); }},
        {LookUpKeys::bitrate, [](auto& d){ return int32ToBitrate(d.bitrate); }},
        {LookUpKeys::filesize, [](auto& d){ return int64ToFileSize(d.fileSize); }},
        {LookUpKeys::filename, [](auto& d){ return toQString(d.fileName); }},
        {LookUpKeys::addedat, [](auto& d){ return int64ToDateISO(d.addedAt); }},
    };
    albumFields = {
        {LookUpKeys::album, [](auto& d){ return toQString(d.name); }},
        {LookUpKeys::albumartist, [](auto& d){ return toQString(d.albumArtist); }},
        {LookUpKeys::year, [](auto& d){ return int32ToYear(d.productionYear); }},
        {LookUpKeys::runtime, [](auto& d){ return int64ToRuntimeMinutes(d.runtime); }},
        {LookUpKeys::genres, [max_genres](auto& d){ return joinList(d.genres, max_genres); }},
        {LookUpKeys::musicbrainzid, [](auto& d){ return toQString(d.musicBrainzId); }},
        {LookUpKeys::addedat, [](auto& d){ return int64ToDateISO(d.addedAt); }},
    };
    audioFields = {
        {LookUpKeys::track, [](auto& d){ return toQString(d.name); }},
        {LookUpKeys::year, [](auto& d){ return int32ToYear(d.productionYear); }},
        {LookUpKeys::runtime, [](auto& d){ return int64ToRuntimeMinutes(d.runtime); }},
        {LookUpKeys::albumartist, [](auto& d){ return toQString(d.albumArtist); }},
        {LookUpKeys::genres, [max_genres](auto& d){ return joinList(d.genres, max_genres); }},
        {LookUpKeys::container, [](auto& d){ return toQString(d.container); }},
        {LookUpKeys::audiocodec, [](auto& d){ return toQString(d.audioCodec); }},
        {LookUpKeys::bitrate, [](auto& d){ return int32ToBitrate(d.bitrate); }},
        {LookUpKeys::filesize, [](auto& d){ return int64ToFileSize(d.fileSize); }},
        {LookUpKeys::filename, [](auto& d){ return toQString(d.fileName); }},
        {LookUpKeys::addedat, [](auto& d){ return int64ToDateISO(d.addedAt); }},
    };
}

bool ExcelExport::doExport(QAbstractItemModel* model, EmbyCollection collection, QString fileName) {
    int row = 1;
    QVector<ExportHeaders> cols = createHeaders(collection.type);
    QXlsx::Document xlsx;
    xlsx.addSheet(collection.name);
    QXlsx::Format fmt;
    // ---Cell formats---
    QXlsx::Format headerFmt;
    headerFmt.setFontBold(true);
    headerFmt.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    headerFmt.setPatternBackgroundColor(DEF_HEADER_COLOR);
    QXlsx::Format defaultFmtL;
    defaultFmtL.setFontBold(false);
    defaultFmtL.setHorizontalAlignment(QXlsx::Format::AlignLeft);
    defaultFmtL.setPatternBackgroundColor(DEF_DEFAULT_COLOR);
    defaultFmtL.setTextWrap(true);
    QXlsx::Format seriesFmtL;
    seriesFmtL.setFontBold(false);
    seriesFmtL.setHorizontalAlignment(QXlsx::Format::AlignLeft);
    seriesFmtL.setPatternBackgroundColor(DEF_SERIES_COLOR);
    seriesFmtL.setTextWrap(true);
    QXlsx::Format seasonFmtL;
    seasonFmtL.setFontBold(false);
    seasonFmtL.setHorizontalAlignment(QXlsx::Format::AlignLeft);
    seasonFmtL.setPatternBackgroundColor(DEF_SEASON_COLOR);
    seasonFmtL.setTextWrap(true);
    QXlsx::Format albumFmtL;
    albumFmtL.setFontBold(false);
    albumFmtL.setHorizontalAlignment(QXlsx::Format::AlignLeft);
    albumFmtL.setPatternBackgroundColor(DEF_ALBUM_COLOR);
    albumFmtL.setTextWrap(true);
    QXlsx::Format defaultFmtR;
    defaultFmtR.setFontBold(false);
    defaultFmtR.setHorizontalAlignment(QXlsx::Format::AlignRight);
    defaultFmtR.setPatternBackgroundColor(DEF_DEFAULT_COLOR);
    defaultFmtR.setTextWrap(true);
    QXlsx::Format seriesFmtR;
    seriesFmtR.setFontBold(false);
    seriesFmtR.setHorizontalAlignment(QXlsx::Format::AlignRight);
    seriesFmtR.setPatternBackgroundColor(DEF_SERIES_COLOR);
    seriesFmtR.setTextWrap(true);
    QXlsx::Format seasonFmtR;
    seasonFmtR.setFontBold(false);
    seasonFmtR.setHorizontalAlignment(QXlsx::Format::AlignRight);
    seasonFmtR.setPatternBackgroundColor(DEF_SEASON_COLOR);
    seasonFmtR.setTextWrap(true);
    QXlsx::Format albumFmtR;
    albumFmtR.setFontBold(false);
    albumFmtR.setHorizontalAlignment(QXlsx::Format::AlignRight);
    albumFmtR.setPatternBackgroundColor(DEF_ALBUM_COLOR);
    albumFmtR.setTextWrap(true);
    // ---Write column header---
    for (int i = 0; i < cols.size(); ++i) {
        xlsx.setColumnWidth(i + 1, cols[i].headerWidth);
        xlsx.write(row, i + 1, cols[i].headerText, headerFmt);
    }
    // ---Movie Model---
    if (auto movieModel = qobject_cast<const MovieTreeModel*>(model)) {
        const std::vector<MovieTreeModel::MovieNode*>* movieNodes = movieModel->rootNodes();
        for (auto& movieNode : *movieNodes) {
            ++row;
            for (int i = 0; i < cols.size(); ++i) {
                fmt = (cols[i].align == l) ? defaultFmtL : defaultFmtR;
                QString data = getFieldValueForExport(movieFields, movieNode->data, cols[i].lookupKey);
                xlsx.write(row, i + 1, data, fmt);
            }
        }
    }
    // --Series Model---
    else if (auto seriesModel = qobject_cast<const SeriesTreeModel*>(model)) {
        const std::vector<SeriesTreeModel::SeriesNode*>* seriesNodes = seriesModel->rootNodes();
        for (auto& seriesNode : *seriesNodes) {
            ++row;
            for (int i = 0; i < cols.size(); ++i) {
                fmt = (cols[i].align == l) ? seriesFmtL : seriesFmtR;
                QString data = getFieldValueForExport(seriesFields, seriesNode->data, cols[i].lookupKey);
                xlsx.write(row, i + 1, data, fmt);
            }
            for (auto& seasonNode : seriesNode->seasons) {
                ++row;
                for (int i = 0; i < cols.size(); ++i) {
                    fmt = (cols[i].align == l) ? seasonFmtL : seasonFmtR;
                    QString data = getFieldValueForExport(seasonFields, seasonNode->data, cols[i].lookupKey);
                    xlsx.write(row, i + 1, data, fmt);
                }
                for (auto& episodeNode : seasonNode->episodes) {
                    ++row;
                    for (int i = 0; i < cols.size(); ++i) {
                        fmt = (cols[i].align == l) ? defaultFmtL : defaultFmtR;
                        QString data = getFieldValueForExport(episodeFields, episodeNode->data, cols[i].lookupKey);
                        xlsx.write(row, i + 1, data, fmt);
                    }
                }
            }
        }
    }
    // ---HomeVideos Model---
    else if (auto homeVideoModel = qobject_cast<const HomeVideoTreeModel*>(model)) {
        const std::vector<HomeVideoTreeModel::HomeVideoNode*>* videoNodes = homeVideoModel->rootNodes();
        for (auto& videoNode : *videoNodes) {
            ++row;
            for (int i = 0; i < cols.size(); ++i) {
                fmt = (cols[i].align == l) ? defaultFmtL : defaultFmtR;
                QString data = getFieldValueForExport(homeVideoFields, videoNode->data, cols[i].lookupKey);
                xlsx.write(row, i + 1, data, fmt);
            }
        }
    }
    // ---MusicVideos Model---
    else if (auto musicVideoModel = qobject_cast<const MusicVideoTreeModel*>(model)) {
        const std::vector<MusicVideoTreeModel::MusicVideoNode*>* videoNodes = musicVideoModel->rootNodes();
        for (auto& videoNode : *videoNodes) {
            ++row;
            for (int i = 0; i < cols.size(); ++i) {
                fmt = (cols[i].align == l) ? defaultFmtL : defaultFmtR;
                QString data = getFieldValueForExport(musicVideoFields, videoNode->data, cols[i].lookupKey);
                xlsx.write(row, i + 1, data, fmt);
            }
        }
    }
    // ---Music Model---
    else if (auto musicModel = qobject_cast<const MusicTreeModel*>(model)) {
        const std::vector<MusicTreeModel::AlbumNode*>* albumNodes = musicModel->rootNodes();
        for (auto& albumNode : *albumNodes) {
            ++row;
            for (int i = 0; i < cols.size(); ++i) {
                fmt = (cols[i].align == l) ? albumFmtL : albumFmtR;
                QString data = getFieldValueForExport(albumFields, albumNode->data, cols[i].lookupKey);
                xlsx.write(row, i + 1, data, fmt);
            }
            for (auto& trackNode : albumNode->tracks) {
                ++row;
                for (int i = 0; i < cols.size(); ++i) {
                    fmt = (cols[i].align == l) ? defaultFmtL : defaultFmtR;
                    QString data = getFieldValueForExport(audioFields, trackNode->data, cols[i].lookupKey);
                    xlsx.write(row, i + 1, data, fmt);
                }
            }
        }
    }
    else return false;
    return xlsx.saveAs(fileName);
}

// ---Header definitions---
QVector<ExcelExport::ExportHeaders> ExcelExport::createHeaders(QString collectionType) {
    if (collectionType == CollectionMovies) {
        return {
            {LookUpKeys::title, c_txt_title, 50, l},
            {LookUpKeys::originaltitle, c_txt_originaltitle, 50, l},
            {LookUpKeys::year, c_txt_year, 10, r},
            {LookUpKeys::runtime, c_txt_runtime, 10, r},
            {LookUpKeys::resolution, c_txt_resolution, 10, r},
            {LookUpKeys::cast, c_txt_cast, 90, l},
            {LookUpKeys::directors, c_txt_directors, 50, l},
            {LookUpKeys::studios, c_txt_studios, 50, l},
            {LookUpKeys::genres, c_txt_genres, 50, l},
            {LookUpKeys::container, c_txt_container, 10, l},
            {LookUpKeys::videocodec, c_txt_videocodec, 15, l},
            {LookUpKeys::audiocodec, c_txt_audiocodec, 15, l},
            {LookUpKeys::bitrate, c_txt_bitrate, 15, r},
            {LookUpKeys::imdbid, c_txt_imdbid, 15, l},
            {LookUpKeys::filesize, c_txt_filesize, 15, r},
            {LookUpKeys::filename, c_txt_filename, 80, l},
            {LookUpKeys::addedat, c_txt_addedat, 15, r},
        };
    } else if (collectionType == CollectionSeries) {
        return {
            {LookUpKeys::series, c_txt_series, 50, l},
            {LookUpKeys::season, c_txt_season, 20, l},
            {LookUpKeys::episode, c_txt_episode, 50, l},
            {LookUpKeys::year, c_txt_year, 10, r},
            {LookUpKeys::runtime, c_txt_runtime, 10, r},
            {LookUpKeys::resolution, c_txt_resolution, 10, r},
            {LookUpKeys::cast, c_txt_cast, 90, l},
            {LookUpKeys::directors, c_txt_directors, 50, l},
            {LookUpKeys::studios, c_txt_studios, 50, l},
            {LookUpKeys::genres, c_txt_genres, 50, l},
            {LookUpKeys::container, c_txt_container, 10, l},
            {LookUpKeys::videocodec, c_txt_videocodec, 15, l},
            {LookUpKeys::audiocodec, c_txt_audiocodec, 10, l},
            {LookUpKeys::bitrate, c_txt_bitrate, 15, r},
            {LookUpKeys::imdbid, c_txt_imdbid, 15, l},
            {LookUpKeys::filesize, c_txt_filesize, 15, r},
            {LookUpKeys::filename, c_txt_filename, 80, l},
            {LookUpKeys::addedat, c_txt_addedat, 15, r},
        };
    } else if (collectionType == CollectionHomeVideos) {
        return {
            {LookUpKeys::title, c_txt_title, 50, l},
            {LookUpKeys::year, c_txt_year, 10, r},
            {LookUpKeys::runtime, c_txt_runtime, 10, r},
            {LookUpKeys::resolution, c_txt_resolution, 10, r},
            {LookUpKeys::people, c_txt_people, 50, l},
            {LookUpKeys::genres, c_txt_genres, 50, l},
            {LookUpKeys::container, c_txt_container, 10, l},
            {LookUpKeys::videocodec, c_txt_videocodec, 15, l},
            {LookUpKeys::audiocodec, c_txt_audiocodec, 15, l},
            {LookUpKeys::bitrate, c_txt_bitrate, 15, r},
            {LookUpKeys::filesize, c_txt_filesize, 15, r},
            {LookUpKeys::filename, c_txt_filename, 80, l},
            {LookUpKeys::addedat, c_txt_addedat, 15, r},
        };
    } else if (collectionType == CollectionMusicVideos) {
        return {
            {LookUpKeys::title, c_txt_title, 50, l},
            {LookUpKeys::year, c_txt_year, 10, r},
            {LookUpKeys::runtime, c_txt_runtime, 10, r},
            {LookUpKeys::resolution, c_txt_resolution, 10, r},
            {LookUpKeys::artists, c_txt_artists, 70, l},
            {LookUpKeys::genres, c_txt_genres, 50, l},
            {LookUpKeys::container, c_txt_container, 10, l},
            {LookUpKeys::videocodec, c_txt_videocodec, 15, l},
            {LookUpKeys::audiocodec, c_txt_audiocodec, 15, l},
            {LookUpKeys::bitrate, c_txt_bitrate, 15, r},
            {LookUpKeys::filesize, c_txt_filesize, 15, r},
            {LookUpKeys::filename, c_txt_filename, 80, l},
            {LookUpKeys::addedat, c_txt_addedat, 15, r},
        };
    } else if (collectionType == CollectionMusic) {
        return {
            {LookUpKeys::album, c_txt_album, 50, l},
            {LookUpKeys::track, c_txt_track, 50, l},
            {LookUpKeys::albumartist, c_txt_albumartist, 50, l},
            {LookUpKeys::year, c_txt_year, 10, r},
            {LookUpKeys::runtime, c_txt_runtime, 10, r},
            {LookUpKeys::genres, c_txt_genres, 50, l},
            {LookUpKeys::container, c_txt_container, 10, l},
            {LookUpKeys::audiocodec, c_txt_audiocodec, 15, l},
            {LookUpKeys::bitrate, c_txt_bitrate, 15, r},
            {LookUpKeys::filesize, c_txt_filesize, 15, r},
            {LookUpKeys::filename, c_txt_filename, 50, l},
            {LookUpKeys::addedat, c_txt_addedat, 15, r},
        };
    };
    return {};
}

