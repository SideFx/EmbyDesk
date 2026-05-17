/////////////////////////////////////////////////////////////////////////////
// Name:        jbpreferences.h
// Purpose:     App preferences (header)
// Author:      Jan Buchholz
// Created:     2025-11-19
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QObject>
#include <QJsonObject>
#include <QVariant>
#include <QVariantMap>

class JBPreferences : public QObject {
    Q_OBJECT

public:
    explicit JBPreferences(QObject *parent = nullptr);
    ~JBPreferences();
    QString GetPreferencesDefaultLocation();
    bool SavePreferences(QString filePath, QString orgName, QString appName);
    bool LoadPreferences(QString filePath, QString orgName, QString appName);
    bool SavePreferencesToDefaultLocation(QString orgName, QString appName);
    bool LoadPreferencesFromDefaultLocation(QString orgName, QString appName);
    void PushArray(QString key, QByteArray values);
    void PushString(QString key, QString value);
    void PushNumber(QString key, quint64 value);
    void PushBoolean(QString key, bool value);
    void PushFont(QString key, QFont font);
    QByteArray PopArray(QString key);
    QString PopString(QString key);
    quint64 PopNumber(QString key);
    bool PopBoolean(QString key);
    QFont PopFont(QString key);

private:
    QVariantMap m_prefs;
    QByteArray ascii85Encode(const QByteArray ba);
    QByteArray ascii85Decode(const QByteArray ba, bool *error);
    QString createPath(QString path, QString orgName, QString appName);
    const quint8 cZero = 'z';
    const quint8 cBase = 33;

};

