/////////////////////////////////////////////////////////////////////////////
// Name:        seriestreemodel.cpp
// Purpose:     Item model for Emby "tvshows"
// Author:      Jan Buchholz
// Created:     2026-05-01
// Changed:     2026-05-16
/////////////////////////////////////////////////////////////////////////////

#include "seriestreemodel.h"
#include "headertexts.h"
#include "../conversions.hpp"

SeriesTreeModel::SeriesTreeModel(QObject *parent) : QAbstractItemModel{parent} {}

void SeriesTreeModel::setData(const SeriesData& data) {
    beginResetModel();
    clear();
    // ---Loop over Series---
    for (const auto &series : data.tSeriesData) {
        auto *seriesNode = new SeriesNode;
        seriesNode->data = series;
        // ---Seasons for current Series---
        for (const auto &season : data.tSeasonData) {
            if (season.seriesId != series.seriesId) continue;
            auto *seasonNode = new SeasonNode;
            seasonNode->data = season;
            seasonNode->parent = seriesNode;
            // ---Episodes for current Season---
            for (const auto &episode : data.tEpisodeData) {
                if (episode.seasonId != season.seasonId ||
                    episode.seriesId != season.seriesId) continue;
                auto *episodeNode = new EpisodeNode;
                episodeNode->data = episode;
                episodeNode->parent = seasonNode;
                seasonNode->episodes.push_back(episodeNode);
            }
            // ---Sort Episodes by sortIndex---
            std::sort(seasonNode->episodes.begin(), seasonNode->episodes.end(),
                      [](auto *a, auto *b) {
                          return a->data.sortIndex < b->data.sortIndex;
                      });
            seriesNode->seasons.push_back(seasonNode);
        }
        // ---Sort Seasons by sortIndex---
        std::sort(seriesNode->seasons.begin(), seriesNode->seasons.end(),
                  [](auto *a, auto *b) {
                      return a->data.sortIndex < b->data.sortIndex;
                  });
        m_series.push_back(seriesNode);
    }
    // ---Sort Series by name---
    std::sort(m_series.begin(), m_series.end(),
              [](auto *a, auto *b) {
                  return a->data.name < b->data.name;
              });
    endResetModel();
}

QModelIndex SeriesTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (row < 0 || column < 0 || column >= ColumnCount) return {};
    if (!parent.isValid()) {
        if (row >= (int)m_series.size()) return {};
        return createIndex(row, column, static_cast<Node*>(m_series[row]));
    }
    Node *node = static_cast<Node*>(parent.internalPointer());
    switch (node->type) {
    case Node::Type::Series: {
        auto *series = static_cast<SeriesNode*>(node);
        if (row >= (int)series->seasons.size()) return {};
        return createIndex(row, column, static_cast<Node*>(series->seasons[row]));
    }
    case Node::Type::Season: {
        auto *season = static_cast<SeasonNode*>(node);
        if (row >= (int)season->episodes.size()) return {};
        return createIndex(row, column, static_cast<Node*>(season->episodes[row]));
    }
    case Node::Type::Episode: return {};
    }
    return {};
}

QModelIndex SeriesTreeModel::parent(const QModelIndex& child) const {
    if (!child.isValid()) return {};
    Node *node = static_cast<Node*>(child.internalPointer());
    if (!node || !node->parent) return {};
    Node *parentNode = node->parent;
    switch (parentNode->type) {
        case Node::Type::Series: {
            auto *series = static_cast<SeriesNode*>(parentNode);
            int row = std::find(m_series.begin(), m_series.end(), series) - m_series.begin();
            return createIndex(row, 0, parentNode);
        }
        case Node::Type::Season: {
            auto *season = static_cast<SeasonNode*>(parentNode);
            auto *series = static_cast<SeriesNode*>(season->parent);
            int row = std::find(series->seasons.begin(), series->seasons.end(), season)
                                - series->seasons.begin();
            return createIndex(row, 0, parentNode);
        }
        case Node::Type::Episode: return {};
    }
    return {};
}

int SeriesTreeModel::rowCount(const QModelIndex& parent) const {
    if (!parent.isValid()) return static_cast<int>(m_series.size());
    Node *node = static_cast<Node*>(parent.internalPointer());
    switch (node->type) {
        case Node::Type::Series: {
            auto *series = static_cast<SeriesNode*>(node);
            return static_cast<int>(series->seasons.size());
        }
        case Node::Type::Season: {
            auto *season = static_cast<SeasonNode*>(node);
            return static_cast<int>(season->episodes.size());
        }
        case Node::Type::Episode: return 0;
    }
    return 0;
}

int SeriesTreeModel::columnCount(const QModelIndex& parent) const {
    return ColumnCount;
}

QVariant SeriesTreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return {};
    Node *node = static_cast<Node*>(index.internalPointer());
    if (role == Qt::DisplayRole) {
        switch (node->type) {
            // ---EpisodeNode---
            case Node::Type::Episode: {
                auto *episode = static_cast<EpisodeNode*>(node);
                switch (index.column()) {
                    case Name:          return toQString(episode->data.name);
                    case Year:          return int32ToYear(episode->data.productionYear);
                    case Runtime:       return int64ToRuntimeMinutes(episode->data.runtime);
                    case Container:     return toQString(episode->data.container);
                    case VideoCodec:    return toQString(episode->data.videoCodec);
                    case AudioCodec:    return toQString(episode->data.audioCodec);
                    case Resolution:    return resolutionValue(episode->data.width, episode->data.height);
                    case Bitrate:       return int32ToBitrate(episode->data.bitrate);
                    case ImDBId:        return toQString(episode->data.imDbId);
                    case FileSize:      return int64ToFileSize(episode->data.fileSize);
                    case FileName:      return toQString(episode->data.fileName);
                    case AddedAt:       return int64ToDateISO(episode->data.addedAt);
                    default: return {};
                }
            }
            // ---SeasonNode---
            case Node::Type::Season: {
                auto *season = static_cast<SeasonNode*>(node);
                switch (index.column()) {
                    case Name:    return toQString(season->data.name);
                    case Year:    return int32ToYear(season->data.productionYear);
                    case Runtime: return int64ToRuntimeMinutes(season->data.runtime);
                    case AddedAt: return int64ToDateISO(season->data.addedAt);
                    default: return {};
                }
            }
            // ---SeriesNode---
            case Node::Type::Series: {
                auto *series = static_cast<SeriesNode*>(node);
                switch (index.column()) {
                    case Name:          return toQString(series->data.name);
                    case Year:          return int32ToYear(series->data.productionYear);
                    case ImDBId:        return toQString(series->data.imDbId);
                    case AddedAt:       return int64ToDateISO(series->data.addedAt);
                    default: return {};
                }
            }
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

QVariant SeriesTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
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
        case FileSize:      return c_txt_filesize;
        case FileName:      return c_txt_filename;
        case AddedAt:       return c_txt_addedat;
    }
    return {};
}

SeriesTreeModel::Node* SeriesTreeModel::itemAt(const QModelIndex& index) const {
    if (!index.isValid()) return nullptr;
    return static_cast<Node*>(index.internalPointer());
}

const SeriesDataInc* SeriesTreeModel::seriesData(const Node* node) const {
    if (node->type != Node::Type::Series) return nullptr;
    return &static_cast<const SeriesNode*>(node)->data;
}

const SeasonDataInc* SeriesTreeModel::seasonData(const Node* node) const {
    if (node->type != Node::Type::Season) return nullptr;
    return &static_cast<const SeasonNode*>(node)->data;
}

const QStringList SeriesTreeModel::episodeNamesForSeason(const Node* node) const {
    if (node->type != Node::Type::Season) return {};
    QStringList names;
    const SeasonNode *season = static_cast<const SeasonNode*>(node);
    for (auto& episode : season->episodes) names.append(toQString(episode->data.name));
    return names;
}

const EpisodeDataInc* SeriesTreeModel::episodeData(const Node* node) const {
    if (node->type != Node::Type::Episode) return nullptr;
    return &static_cast<const EpisodeNode*>(node)->data;
}

const std::vector<SeriesTreeModel::SeriesNode*>* SeriesTreeModel::rootNodes() const {
    return &m_series;
}

void SeriesTreeModel::clear() {
    for (auto *series : m_series) {
        for (auto *season : series->seasons) {
            for (auto *episode : season->episodes) {
                delete episode;
            }
            delete season;
        }
        delete series;
    }
    m_series.clear();
}

void SeriesTreeModel::clearModel() {
    beginResetModel();
    clear();
    endResetModel();
}
