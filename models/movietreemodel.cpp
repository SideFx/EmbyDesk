/////////////////////////////////////////////////////////////////////////////
// Name:        movietreemodel.cpp
// Purpose:     Item model for Emby "movies"
// Author:      Jan Buchholz
// Created:     2026-05-01
// Changed:     2026-05-18
/////////////////////////////////////////////////////////////////////////////

#include "movietreemodel.h"
#include "headertexts.h"
#include "../conversions.hpp"

MovieTreeModel::MovieTreeModel(QObject *parent) : QAbstractItemModel{parent} {}

void MovieTreeModel::setData(const MovieData& data) {
    beginResetModel();
    clear();
    for (const auto &movie : data.tMovieData) {
        auto* movieNode = new MovieNode;
        movieNode->data = movie;
        for (const auto &folder : data.tFolderData) {
            if (folder.folderId == movie.folderId) {
                movieNode->folderName = folder.name;
                break;
            }
        }
        m_movies.push_back(movieNode);
        m_stat.l1++;
    }
    endResetModel();
}

QModelIndex MovieTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (row < 0 || column < 0 || column >= ColumnCount) return {};
    if (!parent.isValid()) {
        if (row >= (int)m_movies.size()) return {};
        return createIndex(row, column, static_cast<Node*>(m_movies[row]));
    }
    return {};
}

QModelIndex MovieTreeModel::parent(const QModelIndex&) const {
    return QModelIndex();
}

int MovieTreeModel::rowCount(const QModelIndex& parent) const {
    if (!parent.isValid()) return static_cast<int>(m_movies.size());
    return 0;
}

int MovieTreeModel::columnCount(const QModelIndex&) const {
    return ColumnCount;
}

QVariant MovieTreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return {};
    Node *node = static_cast<Node*>(index.internalPointer());
    if (node->type != Node::Type::Movie) return {};
    auto *movie = static_cast<MovieNode*>(node);
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case Name:          return toQString(movie->data.name);
            case Year:          return int32ToYear(movie->data.productionYear);
            case Runtime:       return int64ToRuntimeMinutes(movie->data.runtime);
            case Container:     return toQString(movie->data.container);
            case VideoCodec:    return toQString(movie->data.videoCodec);
            case AudioCodec:    return toQString(movie->data.audioCodec);
            case Resolution:    return resolutionValue(movie->data.width, movie->data.height);
            case Bitrate:       return int32ToBitrate(movie->data.bitrate);
            case ImDBId:        return toQString(movie->data.imDbId);
            case FolderName:    return toQString(movie->folderName);
            case FileSize:      return int64ToFileSize(movie->data.fileSize);
            case FileName:      return toQString(movie->data.fileName);
            case AddedAt:       return int64ToDateISO(movie->data.addedAt);
        }
    }
    if (role == SortRole) {
        switch (index.column()) {
            case Name: return toQString(movie->data.name);
            case Year: return movie->data.productionYear;
            case Runtime: return movie->data.runtime;
            case Container: return toQString(movie->data.container);
            case VideoCodec: return toQString(movie->data.videoCodec);
            case AudioCodec: return toQString(movie->data.audioCodec);
            case Resolution: return resolutionKey(movie->data.width, movie->data.height);
            case Bitrate: return movie->data.bitrate;
            case ImDBId: return toQString(movie->data.imDbId);
            case FolderName: return toQString(movie->folderName);
            case FileSize: return movie->data.fileSize;
            case FileName: return toQString(movie->data.fileName);
            case AddedAt: return movie->data.addedAt;
        }
    }
    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
            case Year:
            case Resolution:
            case Runtime:
            case Bitrate:
            case FileSize:
                return QVariant::fromValue(Qt::AlignRight | Qt::AlignVCenter);
        }
    }
    return {};
}

QVariant MovieTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return {};
    switch (section) {
        case Name:          return c_txt_title;
        case Year:          return c_txt_year;
        case Runtime:       return c_txt_runtime;
        case Container:     return c_txt_container;
        case VideoCodec:    return c_txt_videocodec;
        case AudioCodec:    return c_txt_audiocodec;
        case Resolution:    return c_txt_resolution;
        case Bitrate:       return c_txt_bitrate;
        case ImDBId:        return c_txt_imdbid;
        case FolderName:    return c_txt_foldername;
        case FileSize:      return c_txt_filesize;
        case FileName:      return c_txt_filename;
        case AddedAt:       return c_txt_addedat;
    }
    return {};
}

MovieTreeModel::Node* MovieTreeModel::itemAt(const QModelIndex& index) const {
    if (!index.isValid()) return nullptr;
    return static_cast<Node*>(index.internalPointer());
}

const MovieDataInc* MovieTreeModel::movieData(const MovieTreeModel::Node* node) const {
    if (node->type != Node::Type::Movie) return nullptr;
    return &static_cast<const MovieNode*>(node)->data;
}

const std::vector<MovieTreeModel::MovieNode*>* MovieTreeModel::rootNodes() const {
    return &m_movies;
}

void MovieTreeModel::clear() {
    m_movies.clear();
    m_stat = {};
}

void MovieTreeModel::clearModel() {
    beginResetModel();
    clear();
    endResetModel();
}

const QString MovieTreeModel::getStatistics() {
    return "M: " + QString::number(m_stat.l1);
}
