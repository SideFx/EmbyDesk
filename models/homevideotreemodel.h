/////////////////////////////////////////////////////////////////////////////
// Name:        homevideotreemodel.h
// Purpose:     Item model for Emby "homevideos"
// Author:      Jan Buchholz
// Created:     2026-05-01
// Changed:     2026-05-09
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QAbstractItemModel>
#include "jbparser.hpp"

class HomeVideoTreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit HomeVideoTreeModel(QObject *parent = nullptr);

    enum Column {
        Name = 0,
        FolderName,
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
        ColumnCount
    };
    enum Roles {
        SortRole = Qt::UserRole + 1
    };
    struct Node {
        enum class Type { HomeVideo };
        Type type;
        Node *parent = nullptr;
        virtual ~Node() = default;
    };
    struct HomeVideoNode : Node {
        HomeVideoDataInc data;
        std::string folderName;
        HomeVideoNode() { type = Type::HomeVideo; }
    };

    void setData(const HomeVideoData& data);
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex&) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Node* itemAt(const QModelIndex& index = QModelIndex()) const;
    const HomeVideoDataInc* homeVideoData(const Node* node) const;
    const std::vector<HomeVideoTreeModel::HomeVideoNode*>* rootNodes() const;
    void clearModel();

private:
    std::vector<HomeVideoNode*> m_videos;

    void clear();
};


