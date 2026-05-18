/////////////////////////////////////////////////////////////////////////////
// Name:        movietreemodel.h
// Purpose:     Item model for Emby "movies"
// Author:      Jan Buchholz
// Created:     2026-05-01
// Changed:     2026-05-18
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QAbstractItemModel>
#include "jbparser.hpp"
#include "statistics.h"

class MovieTreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit MovieTreeModel(QObject *parent = nullptr);

    enum Column {
        Name = 0,
        Year,
        Runtime,
        Container,
        VideoCodec,
        AudioCodec,
        Resolution,
        Bitrate,
        FolderName,
        FileSize,
        FileName,
        AddedAt,
        ImDBId,
        ColumnCount
    };
    enum Roles {
        SortRole = Qt::UserRole + 1
    };
    struct Node {
        enum class Type { Movie };
        Type type;
        Node *parent = nullptr;
        virtual ~Node() = default;
    };
    struct MovieNode : Node {
        MovieDataInc data;
        std::string folderName;
        MovieNode() { type = Type::Movie; }
    };

    void setData(const MovieData& data);
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex&) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Node* itemAt(const QModelIndex& index = QModelIndex()) const;
    const MovieDataInc* movieData(const Node* node) const;
    const std::vector<MovieTreeModel::MovieNode*>* rootNodes() const;
    void clearModel();
    const QString getStatistics();

private:
    std::vector<MovieNode*> m_movies;
    Statistics m_stat;

    void clear();
};


