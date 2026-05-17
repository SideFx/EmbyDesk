/////////////////////////////////////////////////////////////////////////////
// Name:        jbpreferences.cpp
// Purpose:     App preferences
// Author:      Jan Buchholz
// Created:     2025-11-19
/////////////////////////////////////////////////////////////////////////////

#include "jbpreferences.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QByteArray>
#include <QString>
#include <QFont>
#include <QIODevice>
#include <QDir>
#include <QStandardPaths>
#include <QApplication>

JBPreferences::JBPreferences(QObject *parent) : QObject{parent} {
    m_prefs.clear();
}

JBPreferences::~JBPreferences() {}

QString JBPreferences::GetPreferencesDefaultLocation() {
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
}

void JBPreferences::PushArray(QString key, QByteArray values) {
    m_prefs.insert(key, QString(ascii85Encode(values)));
}

void JBPreferences::PushString(QString key, QString value) {
    QByteArray ba = value.toUtf8();
    m_prefs.insert(key, ascii85Encode(ba));
}

void JBPreferences::PushNumber(QString key, quint64 value) {
    PushString(key, QString::number(value));
}

void JBPreferences::PushBoolean(QString key, bool value) {
    PushString(key, (value) ? "1" : "0");
}

void JBPreferences::PushFont(QString key, QFont font) {
    PushString(key, font.toString());
}

QByteArray JBPreferences::PopArray(QString key) {
    QByteArray ba {};
    bool error;
    for (QVariantMap::iterator iter = m_prefs.begin(); iter != m_prefs.end(); iter++) {
        if (iter.key() == key) {
            QString tmp = iter.value().toString();
            ba = ascii85Decode(tmp.toUtf8(), &error);
            if (error) return {};
            break;
        }
    }
    return ba;
}

QString JBPreferences::PopString(QString key) {
    return QString(PopArray(key));
}

quint64 JBPreferences::PopNumber(QString key) {
    return PopString(key).toULongLong();
}

bool JBPreferences::PopBoolean(QString key) {
    return PopString(key) == "1";
}

QFont JBPreferences::PopFont(QString key) {
    QFont font;
    QString tmp = PopString(key);
    if (!tmp.isEmpty()) font.fromString(tmp); else font = QApplication::font();
    return font;
}

QString JBPreferences::createPath(QString path, QString orgName, QString appName) {
    QString part = orgName + "." + appName + ".json";
    QFileInfo fileInfo(path, part);
    return fileInfo.absoluteFilePath();
}

bool JBPreferences::SavePreferences(QString filePath, QString orgName, QString appName) {
    bool b = false;
    QJsonObject jprefs = QJsonObject::fromVariantMap(m_prefs);
    QJsonDocument doc(jprefs);
    if (!QDir(filePath).exists()) QDir().mkpath(filePath);
    if (!QDir(filePath).exists()) return false;
    QString path = createPath(filePath, orgName, appName);
    QFile file(path);
    b = file.open(QIODevice::WriteOnly);
    if (b) {
        QTextStream out(&file);
        out << doc.toJson() << Qt::endl;
        file.close();
    }
    return b;
}

bool JBPreferences::LoadPreferences(QString filePath, QString orgName, QString appName) {
    m_prefs.clear();
    QJsonObject jprefs;
    QJsonDocument doc;
    QJsonParseError err;
    QString data {};
    QString path = createPath(filePath, orgName, appName);
    QFile file(path);
    if (!file.exists()) return false;
    bool b = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (b) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            data += in.readLine();
        }
        file.close();
        doc = QJsonDocument::fromJson(data.toLocal8Bit(), &err);
        if (err.error != QJsonParseError::NoError) return false;
        b = !doc.isNull();
        if (b) {
            jprefs = doc.object();
            m_prefs = jprefs.toVariantMap();
        }
    }
    return b;
}

bool JBPreferences::SavePreferencesToDefaultLocation(QString orgName, QString appName) {
    return SavePreferences(GetPreferencesDefaultLocation(), orgName, appName);
}

bool JBPreferences::LoadPreferencesFromDefaultLocation(QString orgName, QString appName) {
    return LoadPreferences(GetPreferencesDefaultLocation(), orgName, appName);
}

QByteArray JBPreferences::ascii85Encode(const QByteArray ba) {
    QByteArray result {};
    const quint64 l = ba.size();
    result.reserve(l * 2);
    quint32 number = 0;
    quint64 i = 0;
    int j;
    int pad = 0;
    quint8 enc[5] {};
    uint8_t c;
    while (i < l) {
        for (j = 0; j < 4; j++) {
            if (i < l) c = ba[i++]; else {
                c = 0x00;
                pad++;
            }
            (number <<= 8) |= c;
        }
        if (number > 0) {
            enc[4] = (number % 85) + cBase;
            number /= 85;
            enc[3] = (number % 85) + cBase;
            number /= 85;
            enc[2] = (number % 85) + cBase;
            number /= 85;
            enc[1] = (number % 85) + cBase;
            number /= 85;
            enc[0] = (number % 85) + cBase;
            for (j = 0; j < (4 - pad) + 1; j++) result.append(enc[j]);
        }
        else result.append(cZero);
    }
    return result;
}

QByteArray JBPreferences::ascii85Decode(const QByteArray ba, bool *error) {
    QByteArray result {};
    const quint64 l = ba.size();
    result.reserve(l);
    quint32 number = 0;
    quint64 i = 0;
    int j;
    *error = false;
    while (i < l) {
        if (ba[i] == cZero) {
            for (j = 0; j < 4; j++) result.append(quint8(0));
            i++;
        }
        else {
            number = 0;
            int cnt = 0;
            for (j = 0; j < 5; j++) {
                if (i < l) {
                    if (ba[i] < 33 || ba[i] > 117) {
                        *error = true;
                        return {};
                    }
                    number = number * 85 + (ba[i] - cBase);
                    i++;
                    cnt++;
                }
                else number = number * 85 + 84;
            }
            if (cnt >= 2) result.append(number >> 24);
            if (cnt >= 3) result.append(number >> 16);
            if (cnt >= 4)result.append(number >> 8);
            if (cnt == 5) result.append(number);
        }
    }
    return result;
}

