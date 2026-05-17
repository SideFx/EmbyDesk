/////////////////////////////////////////////////////////////////////////////
// Name:        musicvideotreemodel.h
// Purpose:     Item model for Emby "musicvideos"
// Author:      Jan Buchholz
// Created:     2026-05-01
// Changed:     2026-05-09
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QAbstractItemModel>
#include "jbparser.hpp"

class MusicVideoTreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit MusicVideoTreeModel(QObject *parent = nullptr);

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
        enum class Type { MusicVideo };
        Type type;
        Node *parent = nullptr;
        virtual ~Node() = default;
    };
    struct MusicVideoNode : Node {
        MusicVideoDataInc data;
        std::string folderName;
        MusicVideoNode() { type = Type::MusicVideo; }
    };

    void setData(const MusicVideoData& data);
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex&) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Node* itemAt(const QModelIndex& index = QModelIndex()) const;
    const MusicVideoDataInc* musicVideoData(const Node* node) const;
    const std::vector<MusicVideoTreeModel::MusicVideoNode*>* rootNodes() const;
    void clearModel();

private:
    std::vector<MusicVideoNode*> m_videos;

    void clear();
};


