/////////////////////////////////////////////////////////////////////////////
// Name:        excelexport.h
// Purpose:     Export collection as Excel worksheet
// Author:      Jan Buchholz
// Created:     2026-05-07
// Changed:     2026-05-10
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QStringList>
#include <QAbstractItemModel>
#include "jbparser.hpp"
#include "metatypes.h"

class ExcelExport { 

public:
    ExcelExport();
    bool doExport(QAbstractItemModel* model, EmbyCollection coll, QString fileName);

private:
    template<typename T>
    using FieldGetter = std::function<QString(const T&)>;
    template<typename T>
    using FieldMap = QMap<QString, FieldGetter<T>>;

    FieldMap<MovieDataInc> movieFields;
    FieldMap<SeriesDataInc> seriesFields;
    FieldMap<SeasonDataInc> seasonFields;
    FieldMap<EpisodeDataInc> episodeFields;
    FieldMap<HomeVideoDataInc> homeVideoFields;
    FieldMap<MusicVideoDataInc> musicVideoFields;
    FieldMap<AlbumDataInc> albumFields;
    FieldMap<AudioDataInc> audioFields;

    template<typename T>
    QString getFieldValueForExport(const FieldMap<T>& map, const T& data, const QString& name) {
        auto it = map.find(name);
        if (it != map.end()) return it.value()(data);
        return {};
    }
    enum hAlignment {l, r};
    struct ExportHeaders {
        QString lookupKey;
        QString headerText;
        int headerWidth;
        hAlignment align;
    };

    QVector<ExportHeaders> createHeaders(QString collectionType);
};


