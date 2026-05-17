/////////////////////////////////////////////////////////////////////////////
// Name:        jbimagecache.h
// Purpose:     Local cache for covers, etc.
// Author:      Jan Buchholz
// Created:     2026-05-01
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QPixmap>
#include <QString>
#include <QMap>

class JBImageCache {

public:
    JBImageCache() = delete;
    static QPixmap getPixmap(const std::string& itemId, const std::string& tag);

private:
    static QMap<QString, QPixmap> m_cache;
};


