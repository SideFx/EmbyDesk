/////////////////////////////////////////////////////////////////////////////
// Name:        musictreemodel.cpp
// Purpose:     Item model for Emby "music"
// Author:      Jan Buchholz
// Created:     2026-05-01
// Changed:     2026-05-17
/////////////////////////////////////////////////////////////////////////////

#include "musictreemodel.h"
#include "headertexts.h"
#include "../conversions.hpp"

MusicTreeModel::MusicTreeModel(QObject *parent) : QAbstractItemModel{parent} {}

void MusicTreeModel::setData(const MusicData& data) {
    beginResetModel();
    clear();
    // ---Loop over albums---
    for (const auto& album : data.tAlbumData) {
        auto *albumNode = new AlbumNode;
        albumNode->data = album;
        // ---Tracks for current album---
        for (const auto& audio : data.tAudioData) {
            if (audio.albumId != album.albumId || audio.mediaType != MediaTypeAudio) continue;
            auto *audioNode = new AudioNode;
            audioNode->data = audio;
            audioNode->parent = albumNode;
            albumNode->tracks.push_back(audioNode);
        }
        // ---Sort tracks by discNumber & trackNumber---
        std::stable_sort(albumNode->tracks.begin(), albumNode->tracks.end(),
                  [](auto *a, auto *b) {
                      return a->data.trackNumber < b->data.trackNumber;
                  });
        std::stable_sort(albumNode->tracks.begin(), albumNode->tracks.end(),
                         [](auto *a, auto *b) {
                             return a->data.discNumber < b->data.discNumber;
                         });
        m_albums.push_back(albumNode);
    }
    // ---Sort albums by album artist & name---
    std::stable_sort(m_albums.begin(), m_albums.end(),
                     [](auto *a, auto *b) {
                         return (a->data.name < b->data.name);
                     });
    std::stable_sort(m_albums.begin(), m_albums.end(),
                    [](auto *a, auto *b) {
                        return (a->data.albumArtist < b->data.albumArtist);
                    });
    endResetModel();
}

QModelIndex MusicTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (row < 0 || column < 0 || column >= ColumnCount) return {};
    if (!parent.isValid()) {
        if (row >= (int)m_albums.size()) return {};
        return createIndex(row, column, static_cast<Node*>(m_albums[row]));
    }
    Node *node = static_cast<Node*>(parent.internalPointer());
    switch (node->type) {
        case Node::Type::Album: {
            auto *series = static_cast<AlbumNode*>(node);
            if (row >= (int)series->tracks.size()) return {};
            return createIndex(row, column, static_cast<Node*>(series->tracks[row]));
        }
    case Node::Type::Audio: return {};
    }
    return {};
}

QModelIndex MusicTreeModel::parent(const QModelIndex& child) const {
    if (!child.isValid()) return {};
    Node *node = static_cast<Node*>(child.internalPointer());
    if (!node || !node->parent) return {};
    Node *parentNode = node->parent;
    switch (parentNode->type) {
    case Node::Type::Album: {
        auto *album = static_cast<AlbumNode*>(parentNode);
        int row = std::find(m_albums.begin(), m_albums.end(), album) - m_albums.begin();
        return createIndex(row, 0, parentNode);
    }
    case Node::Type::Audio: return {};
    }
    return {};
}

int MusicTreeModel::rowCount(const QModelIndex& parent) const {
    if (!parent.isValid()) return static_cast<int>(m_albums.size());
    Node *node = static_cast<Node*>(parent.internalPointer());
    switch (node->type) {
        case Node::Type::Album: {
            auto *album = static_cast<AlbumNode*>(node);
            return static_cast<int>(album->tracks.size());
        }
        case Node::Type::Audio: return 0;
    }
    return 0;
}

int MusicTreeModel::columnCount(const QModelIndex& parent) const {
    return ColumnCount;
}

QVariant MusicTreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return {};
    if (role == Qt::DisplayRole) {
        Node *node = static_cast<Node*>(index.internalPointer());
        switch (node->type) {
            // ---AudioNode---
            case Node::Type::Audio: {
                auto* audio = static_cast<AudioNode*>(node);
                switch (index.column()) {
                    case Name:          return toQString(audio->data.name);
                    case Year:          return int32ToYear(audio->data.productionYear);
                    case TrackNumber:   return int32ToTrackNumber(audio->data.discNumber, audio->data.trackNumber);
                    case AlbumArtist:   return toQString(audio->data.albumArtist);
                    case Runtime:       return int64ToRuntimeHMS(audio->data.runtime);
                    case Container:     return toQString(audio->data.container);
                    case AudioCodec:    return toQString(audio->data.audioCodec);
                    case Bitrate:       return int32ToBitrate(audio->data.bitrate);
                    case FileSize:      return int64ToFileSize(audio->data.fileSize);
                    case FileName:      return toQString(audio->data.fileName);
                    case AddedAt:       return int64ToDateISO(audio->data.addedAt);
                    default: return {};
                }
            }
            // ---AlbumNode---
            case Node::Type::Album: {
                auto* album = static_cast<AlbumNode*>(node);
                switch (index.column()) {
                    case Name:          return toQString(album->data.name);
                    case Year:          return int32ToYear(album->data.productionYear);
                    case AlbumArtist:   return toQString(album->data.albumArtist);
                    case Runtime:       return int64ToRuntimeHMS(album->data.runtime);
                    case AddedAt:       return int64ToDateISO(album->data.addedAt);
                    case MusicBrainzId: return toQString(album->data.musicBrainzId);
                    default: return {};
                }
            }
        }
    }
    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
            case Year:
            case Runtime:
            case Bitrate:
            case FileSize:
                return QVariant::fromValue(Qt::AlignRight | Qt::AlignVCenter);
        }
    }
    return {};
}

QVariant MusicTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return {};
    switch (section) {
        case Name:          return c_txt_title;
        case Year:          return c_txt_year;
        case TrackNumber:   return c_txt_tracknumber;
        case Runtime:       return c_txt_runtime;
        case Container:     return c_txt_container;
        case AlbumArtist:   return c_txt_albumartist;
        case AudioCodec:    return c_txt_audiocodec;
        case Bitrate:       return c_txt_bitrate;
        case MusicBrainzId: return c_txt_musicbrainzid;
        case FileSize:      return c_txt_filesize;
        case FileName:      return c_txt_filename;
        case AddedAt:       return c_txt_addedat;
    }
    return {};
}

MusicTreeModel::Node* MusicTreeModel::itemAt(const QModelIndex& index) const {
    if (!index.isValid()) return nullptr;
    return static_cast<Node*>(index.internalPointer());
}

const AlbumDataInc* MusicTreeModel::albumData(const MusicTreeModel::Node* node) const {
    if (node->type != Node::Type::Album) return nullptr;
    return &static_cast<const AlbumNode*>(node)->data;
}

const QStringList MusicTreeModel::trackNamesForAlbum(const Node* node) const {
    if (node->type != Node::Type::Album) return {};
    QStringList names;
    const AlbumNode* album = static_cast<const AlbumNode*>(node);
    for (auto& track : album->tracks) names.append(toQString(track->data.name));
    return names;
}

const AudioDataInc* MusicTreeModel::audioData(const MusicTreeModel::Node* node) const {
    if (node->type != Node::Type::Audio) return nullptr;
    return &static_cast<const AudioNode*>(node)->data;
}

const std::vector<MusicTreeModel::AlbumNode*>* MusicTreeModel::rootNodes() const {
    return &m_albums;
}

void MusicTreeModel::clear() {
    for (auto *album : m_albums) {
        for (auto *audio : album->tracks) {
            delete audio;
        }
        delete album;
    }
    m_albums.clear();
}

void MusicTreeModel::clearModel() {
    beginResetModel();
    clear();
    endResetModel();
}
