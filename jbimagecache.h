/////////////////////////////////////////////////////////////////////////////
// Name:        jbimagecache.h
// Purpose:     Local cache for covers, etc.
// Author:      Jan Buchholz
// Created:     2026-05-21
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QPixmap>
#include <QString>
#include <QMap>
#include <QObject>
#include "sqlreader.h"

class JBImageCache : public QObject {
    Q_OBJECT

public:
    JBImageCache(SqlReader& reader);
    QPixmap getPixmap(const std::string& itemId, const std::string& tag);

private:
    QMap<QString, QPixmap> m_cache;
    SqlReader& m_sqlReader;
};


