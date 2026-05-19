/////////////////////////////////////////////////////////////////////////////
// Name:        metatypes.h
// Purpose:     Struct for Emby collections
// Author:      Jan Buchholz
// Created:     2026-05-19
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QObject>

struct EmbyCollection {
    QString name;
    QString type;
    QString id;
};
Q_DECLARE_METATYPE(EmbyCollection)


