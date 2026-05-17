/////////////////////////////////////////////////////////////////////////////
// Name:        conversions.hpp
// Purpose:     Simple conversion and evaluation functions
// Author:      Jan Buchholz
// Created:     2026-04-26
// Changed:     2026-05-12
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QString>
#include <qdatetime.h>

inline QString toQString(const std::string& s) {
    return QString::fromUtf8(s.c_str());
}

inline QString toQString(std::string_view sv) {
    return QString::fromUtf8(sv.data(), static_cast<int>(sv.size()));
}

inline std::string toStandardString(const QString& s) {
    return s.toUtf8().constData();
}

inline QByteArray toQByteArray(const std::vector<uint8_t>& bytes) {
    return QByteArray(reinterpret_cast<const char*>(bytes.data()),
                      static_cast<int>(bytes.size()));
}

template<typename T>
inline QVector<T> toQVector(const std::vector<T>& v) {
    return QVector<T>(v.begin(), v.end());
}

template<typename T>
inline std::vector<T> toStdVector(const QVector<T>& v) {
    return std::vector<T>(v.begin(), v.end());
}

inline QStringList toQStringList(const std::vector<std::string>& vec) {
    QStringList list;
    list.reserve(static_cast<int>(vec.size()));
    for (const auto& s : vec) list.append(QString::fromStdString(s));
    return list;
}

inline QString int64ToDateISO(int64_t date) {
    QDateTime dt = QDateTime::fromSecsSinceEpoch(date);
    return dt.date().toString("yyyy-MM-dd");
}

inline QString int64ToFileSize(int64_t size) {
    const int64_t KB = 1024;
    const int64_t MB = 1024 * KB;
    const int64_t GB = 1024 * MB;
    if (size < KB) {
        return QString::number(size) + " B";
    } else if (size < MB) {
        double kb = static_cast<double>(size) / KB;
        return QString::asprintf("%.2f KB", kb);
    } else if (size < GB) {
        double mb = static_cast<double>(size) / MB;
        return QString::asprintf("%.2f MB", mb);
    } else {
        double gb = static_cast<double>(size) / GB;
        return QString::asprintf("%.2f GB", gb);
    }
}

inline QString int64ToRuntimeMinutes(int64_t ticks) {
    if (ticks <= 0) return {};
    int64_t seconds = ticks / 10'000'000;
    int64_t minutes = seconds / 60;
    return QString::number(minutes) + " min";
}

inline QString int64ToRuntimeHMS(int64_t ticks) {
    if (ticks <= 0) return "00:00:00";
    int64_t totalSeconds = ticks / 10'000'000;
    int64_t hours   = totalSeconds / 3600;
    int64_t minutes = (totalSeconds % 3600) / 60;
    int64_t seconds = totalSeconds % 60;
    if (hours == 0) {
        return QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }
    return QString("%1:%2:%3")
        .arg(hours,   2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

inline QString int32ToYear(int32_t year) {
    if (year <= 0) return {};
    return QString::number(year);
}

inline QString int32ToBitrate(int32_t bitrate) {
    const int32_t K = 1000;
    const int32_t M = 1000 * K;
    const int32_t G = 1000 * M;
    if (bitrate < K) {
        return QString::number(bitrate) + " b/s";
    } else if (bitrate < M) {
        return QString::number(bitrate / K) + " kb/s";
    } else if (bitrate < G) {
        double mbps = static_cast<double>(bitrate) / M;
        return QString::asprintf("%.2f MB/s", mbps);
    } else {
        double gbps = static_cast<double>(bitrate) / G;
        return QString::asprintf("%.2f GB/s", gbps);
    }
}

inline QString int32ToTrackNumber(int32_t discNo, int32_t trackNo) {
    QString disc = (discNo == 0) ? "1" : QString::number(discNo);
    return disc + "/" + QString::number(trackNo);
}

inline QString resolutionValue(int width, int height) {
    return QString("%1x%2").arg(width).arg(height);
}

inline qint64 resolutionKey(int width, int height) {
    return (static_cast<qint64>(width) << 32) | static_cast<quint32>(height);
}

inline QString joinList(const std::vector<std::string>& vec, const int maxItems) {
    QStringList list;
    int n = vec.size();
    if (n > maxItems) n = maxItems;
    list.reserve(static_cast<int>(n));
    for (const auto& s : vec) {
        list.append(toQString(s));
        --n;
        if (n == 0) break;
    }
    return list.join(", ");
}
