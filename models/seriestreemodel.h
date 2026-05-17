/////////////////////////////////////////////////////////////////////////////
// Name:        seriestreemodel.h
// Purpose:     Item model for Emby "tvshows"
// Author:      Jan Buchholz
// Created:     2026-05-01
// Changed:     2026-05-09
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QAbstractItemModel>
#include "jbparser.hpp"

class SeriesTreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit SeriesTreeModel(QObject *parent = nullptr);

    enum Column {
        Name = 0,
        Year,
        Runtime,
        Container,
        VideoCodec,
        AudioCodec,
        Resolution,
        Bitrate,
        FileSize,
        FileName,
        AddedAt,
        ImDBId,
        ColumnCount
    };
    struct Node {
        enum class Type { Series, Season, Episode };
        Type type;
        Node *parent = nullptr;
        virtual ~Node() = default;
    };
    struct EpisodeNode : Node {
        EpisodeDataInc data;
        EpisodeNode() { type = Type::Episode; }
    };
    struct SeasonNode : Node {
        SeasonDataInc data;
        std::vector<EpisodeNode*> episodes;
        SeasonNode() { type = Type::Season; }
    };
    struct SeriesNode : Node {
        SeriesDataInc data;
        std::vector<SeasonNode*> seasons;
        SeriesNode() { type = Type::Series; }
    };

    void setData(const SeriesData& data);
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Node* itemAt(const QModelIndex& index = QModelIndex()) const;
    const SeriesDataInc* seriesData(const Node* node) const;
    const SeasonDataInc* seasonData(const Node* node) const;
    const EpisodeDataInc* episodeData(const Node* node) const;
    const QStringList episodeNamesForSeason(const Node* node) const;
    const std::vector<SeriesTreeModel::SeriesNode*>* rootNodes() const;
    void clearModel();

private:
    std::vector<SeriesNode*> m_series;

    void clear();
};

