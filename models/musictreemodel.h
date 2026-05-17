/////////////////////////////////////////////////////////////////////////////
// Name:        musictreemodel.h
// Purpose:     Item model for Emby "music"
// Author:      Jan Buchholz
// Created:     2026-05-01
// Changed:     2026-05-09
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QAbstractItemModel>
#include "jbparser.hpp"

class MusicTreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit MusicTreeModel(QObject *parent = nullptr);

    enum Column {
        Name = 0,
        Year,
        TrackNumber,
        AlbumArtist,
        Runtime,
        Container,
        AudioCodec,
        Bitrate,
        FileSize,
        FileName,
        AddedAt,
        MusicBrainzId,
        ColumnCount
    };
    struct Node {
        enum class Type { Album, Audio };
        Type type;
        Node *parent = nullptr;
        virtual ~Node() = default;
    };
    struct AudioNode : Node {
        AudioDataInc data;
        AudioNode() { type = Type::Audio; }
    };
    struct AlbumNode : Node {
        AlbumDataInc data;
        std::vector<AudioNode*> tracks;
        AlbumNode() { type = Type::Album; }
    };

    void setData(const MusicData& data);
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Node* itemAt(const QModelIndex& index = QModelIndex()) const;
    const AlbumDataInc* albumData(const Node* node) const;
    const AudioDataInc* audioData(const Node* node) const;
    const QStringList trackNamesForAlbum(const Node* node) const;
    const std::vector<AlbumNode *> *rootNodes() const;
    void clearModel();

private:
    std::vector<AlbumNode*> m_albums;

    void clear();
};

