/////////////////////////////////////////////////////////////////////////////
// Name:        homevideotreemodel.cpp
// Purpose:     Item model for Emby "homevideos"
// Author:      Jan Buchholz
// Created:     2026-05-01
// Changed:     2026-05-21
/////////////////////////////////////////////////////////////////////////////

#include "homevideotreemodel.h"
#include "headertexts.h"
#include "../conversions.hpp"

HomeVideoTreeModel::HomeVideoTreeModel(QObject *parent) : QAbstractItemModel{parent} {}

void HomeVideoTreeModel::setData(const HomeVideoData& data) {
    beginResetModel();
    clear();
    for (const auto &video : data.tHomeVideoData) {
        auto* videoNode = new HomeVideoNode;
        videoNode->data = video;
        for (const auto &folder : data.tFolderData) {
            if (folder.folderId == video.folderId) {
                videoNode->folderName = folder.name;
                break;
            }
        }
        m_videos.push_back(videoNode);
        m_stat.l1++;
    }
    endResetModel();
}

QModelIndex HomeVideoTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (row < 0 || column < 0 || column >= ColumnCount) return {};
    if (!parent.isValid()) {
        if (row >= (int)m_videos.size()) return {};
        return createIndex(row, column, static_cast<Node*>(m_videos[row]));
    }
    return {};
}

QModelIndex HomeVideoTreeModel::parent(const QModelIndex&) const {
    return QModelIndex();
}

int HomeVideoTreeModel::rowCount(const QModelIndex& parent) const {
    if (!parent.isValid()) return static_cast<int>(m_videos.size());
    return 0;
}

int HomeVideoTreeModel::columnCount(const QModelIndex&) const {
    return ColumnCount;
}

QVariant HomeVideoTreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return {};
    Node *node = static_cast<Node*>(index.internalPointer());
    if (node->type != Node::Type::HomeVideo) return {};
    auto *video = static_cast<HomeVideoNode*>(node);
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case Name:          return toQString(video->data.name);
            case Year:          return int32ToYear(video->data.productionYear);
            case Runtime:       return int64ToRuntimeMinutes(video->data.runtime);
            case Container:     return toQString(video->data.container);
            case VideoCodec:    return toQString(video->data.videoCodec);
            case AudioCodec:    return toQString(video->data.audioCodec);
            case Resolution:    return resolutionValue(video->data.width, video->data.height);
            case Bitrate:       return int32ToBitrate(video->data.bitrate);
            case FolderName:    return toQString(video->folderName);
            case FileSize:      return int64ToFileSize(video->data.fileSize);
            case FileName:      return toQString(video->data.fileName);
            case AddedAt:       return int64ToDateISO(video->data.addedAt);
        }
    }
    if (role == SortRole) {
        switch (index.column()) {
            case Name:       return toQString(video->data.name);
            case Year:       return video->data.productionYear;
            case Runtime:    return video->data.runtime;
            case Container:  return toQString(video->data.container);
            case VideoCodec: return toQString(video->data.videoCodec);
            case AudioCodec: return toQString(video->data.audioCodec);
            case Resolution: return resolutionKey(video->data.width, video->data.height);
            case Bitrate:    return video->data.bitrate;
            case FolderName: return toQString(video->folderName);
            case FileSize:   return video->data.fileSize;
            case FileName:   return toQString(video->data.fileName);
            case AddedAt:    return video->data.addedAt;
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

QVariant HomeVideoTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return {};
    switch (section) {
        case Name:       return c_txt_title;
        case Year:       return c_txt_year;
        case Runtime:    return c_txt_runtime;
        case Container:  return c_txt_container;
        case VideoCodec: return c_txt_videocodec;
        case AudioCodec: return c_txt_videocodec;
        case Resolution: return c_txt_resolution;
        case Bitrate:    return c_txt_bitrate;
        case FolderName: return c_txt_foldername;
        case FileSize:   return c_txt_filesize;
        case FileName:   return c_txt_filename;
        case AddedAt:    return c_txt_addedat;
    }
    return {};
}

HomeVideoTreeModel::Node* HomeVideoTreeModel::itemAt(const QModelIndex& index) const {
    if (!index.isValid()) return nullptr;
    return static_cast<Node*>(index.internalPointer());
}

const HomeVideoDataInc* HomeVideoTreeModel::homeVideoData(const Node* node) const {
    if (node->type != Node::Type::HomeVideo) return nullptr;
    return &static_cast<const HomeVideoNode*>(node)->data;
}

const std::vector<HomeVideoTreeModel::HomeVideoNode*>* HomeVideoTreeModel::rootNodes() const {
    return &m_videos;
}

void HomeVideoTreeModel::clear() {
    m_videos.clear();
    m_stat = {};
}

void HomeVideoTreeModel::clearModel() {
    beginResetModel();
    clear();
    endResetModel();
}

const QString HomeVideoTreeModel::getStatistics() {
    return "V: " + QString::number(m_stat.l1);
}
