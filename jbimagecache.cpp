/////////////////////////////////////////////////////////////////////////////
// Name:        jbimagecache.cpp
// Purpose:     Local cache for covers, etc.
// Author:      Jan Buchholz
// Created:     2026-05-21
/////////////////////////////////////////////////////////////////////////////

#include "jbimagecache.h"
#include "apicall.h"
#include "conversions.hpp"

JBImageCache::JBImageCache(SqlReader& reader) : m_sqlReader(reader) {}

QPixmap JBImageCache::getPixmap(const std::string& itemId, const std::string& tag) {
    QPixmap pix;
    const QString key = toQString(itemId + ":" + tag);
    if (m_cache.contains(key)) {
        return m_cache.value(key);
    } else {
        // ---Not in cache -> load---
        if (tag == DEF_OFFLINE) {
            ByteArrayResult r = m_sqlReader.loadImage(toQString(itemId));
            if (r.code == MSG_OK) {
                QByteArray bytes = toQByteArray(r.bytes);
                pix.loadFromData(bytes);
                m_cache[key] = pix;
            }
            return pix;
        } else {
            ItemImageImp img = APICall::getPrimaryImageForItem(itemId, tag);
            if (img.code != 0) return QPixmap();
            QByteArray bytes = QByteArray::fromBase64(img.imageData.c_str());
            pix.loadFromData(bytes);
            m_cache[key] = pix;
            return pix;
        }
    }
}

