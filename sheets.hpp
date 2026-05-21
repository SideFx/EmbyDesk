/////////////////////////////////////////////////////////////////////////////
// Name:        sheets.hpp
// Purpose:     Sheets for different Emby item types
// Author:      Jan Buchholz
// Created:     2026-05-01
// Changed:     2026-05-21
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QLabel>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QListView>
#include <QStringList>
#include <QStringListModel>
#include <QPainter>
#include "jbparser.hpp"
#include "conversions.hpp"
#include "jbimagecache.h"
#include "globals.h"
#include "models/headertexts.h"

constexpr int c_maxVisibleActors = 5;
constexpr int c_maxVisibleDirectors = 2;
constexpr int c_maxVisibleStudios = 2;
constexpr int c_maxVisibleGenres = 3;
constexpr int c_maxVisibleEpisodes = 13;
constexpr int c_maxVisibleTracks = 15;
constexpr int c_maxVisibleTextLines = 15;
constexpr int c_maxVisibleArtists = 3;
inline QString const dummyText = QObject::tr("No image available");

inline void listViewSetVisibleRows(QListView* lv, int rows) {
    lv->setUniformItemSizes(true);
    int rowHeight = lv->sizeHintForRow(0);
    if (rowHeight <= 0) rowHeight = lv->fontMetrics().height() + 1;
    int rowCount = lv->model()->rowCount();
    int r = (rowCount < rows) ? rowCount : rows;
    if (r <= 0) r = 1;
    lv->setFixedHeight(r * rowHeight + 4);
}

// ---stick label & control together---
inline QWidget* makeGroupWidget(const QString& title, QWidget* control, QWidget* parent) {
    auto* container = new QWidget(parent);
    auto* label = new QLabel(title + ":", container);
    auto* v = new QVBoxLayout(container);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(0);
    v->addWidget(label);
    v->addWidget(control);
    container->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    return container;
}

inline QLabel* makeTitleLabel(QWidget* parent) {
    QLabel* label = new QLabel(parent);
    label->setStyleSheet("font-weight: bold;");
    label->setWordWrap(true);
    return label;
}

inline QLabel* makeCoverLabel(QWidget* parent) {
    QLabel* label = new QLabel(parent);
    label->setStyleSheet("background-color: transparent;");
    label->setScaledContents(false);
    return label;
}

inline QTextBrowser* makeTextBrowser(QWidget* parent) {
    QTextBrowser* browser = new QTextBrowser(parent);
    browser->setOpenExternalLinks(false);
    browser->setReadOnly(true);
    int lineHeight = QFontMetrics(browser->font()).lineSpacing();
    browser->setFixedHeight(c_maxVisibleTextLines * lineHeight);
    return browser;
}

inline QPixmap makeDummyPixmap(int w, int h, const QColor& bg, const QString& text) {
    QPixmap pm(w, h);
    pm.fill(bg);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    QFont font = p.font();
    font.setPointSize(11);
    p.setFont(font);
    p.setPen(QColor(0x808080));
    QRect r(0, 0, w, h);
    p.drawText(r, Qt::AlignCenter, text);
    return pm;
}

class DummyImages {
public:
    static const QPixmap& poster() {
        int w = DEF_IMAGE_HEIGHT * 10 / 14;
        static QPixmap pm = makeDummyPixmap(w, DEF_IMAGE_HEIGHT, QColor(0xC0C0C0), dummyText);
        return pm;
    }
    static const QPixmap& frame() {
        int h = DEF_IMAGE_WIDTH * 9 / 16;
        static QPixmap pm = makeDummyPixmap(DEF_IMAGE_WIDTH, h, QColor(0xC0C0C0), dummyText);
        return pm;
    }
    static const QPixmap& square() {
        static QPixmap pm = makeDummyPixmap(DEF_IMAGE_WIDTH, DEF_IMAGE_HEIGHT,
                                            QColor(0xC0C0C0), dummyText);
        return pm;
    }
};

inline void finalizeSheet(QWidget* sheet) {
    // ---get sheet color---
    const QString bg = sheet->palette().color(QPalette::Window).name();
    // ---remove focus---
    sheet->setFocusPolicy(Qt::NoFocus);
    // ---child controls remove focus & apply color---
    for (auto* child : sheet->findChildren<QWidget*>()) {
        child->setFocusPolicy(Qt::NoFocus);
        if (qobject_cast<QListView*>(child) ||
            qobject_cast<QTextBrowser*>(child)) {
            child->setStyleSheet(child->styleSheet() + " background-color: " + bg + ";");
        }
    }
}

class MovieDetailSheet : public QWidget {
    Q_OBJECT

public:
    explicit MovieDetailSheet(QWidget* parent = nullptr) {
        auto* layout = new QVBoxLayout(this);
        m_title = makeTitleLabel(this);
        layout->addWidget(m_title);
        m_originalTitle = new QLabel(this);
        m_originalTitle->setWordWrap(true);
        layout->addWidget(m_originalTitle);
        m_cover = makeCoverLabel(this);
        layout->addWidget(m_cover);
        m_actorModel = new QStringListModel(this);
        m_actorView  = new QListView(this);
        m_actorView->setModel(m_actorModel);
        layout->addWidget(makeGroupWidget(c_txt_cast, m_actorView, this));
        m_directorModel = new QStringListModel(this);
        m_directorView  = new QListView(this);
        m_directorView->setModel(m_directorModel);
        layout->addWidget(makeGroupWidget(c_txt_directors, m_directorView, this));
        m_studioModel = new QStringListModel(this);
        m_studioView  = new QListView(this);
        m_studioView->setModel(m_studioModel);
        layout->addWidget(makeGroupWidget(c_txt_studios, m_studioView, this));
        m_genreModel = new QStringListModel(this);
        m_genreView  = new QListView(this);
        m_genreView->setModel(m_genreModel);
        layout->addWidget(makeGroupWidget(c_txt_genres, m_genreView, this));
        m_overview = makeTextBrowser(this);
        layout->addWidget(makeGroupWidget(c_txt_overview, m_overview, this));
        layout->addStretch();
        finalizeSheet(this);
    }
    void setData(const MovieDataInc& data, JBImageCache& cache) {
        QPixmap pix;
        m_title->setText(toQString(data.name));
        QString originalTitle = toQString(data.originalTitle);
        if (!originalTitle.isEmpty()) {
            m_originalTitle->setText(originalTitle);
            m_originalTitle->setVisible(true);
        } else m_originalTitle->setVisible(false);
        if (data.primaryImageTag != "") {
            pix = cache.getPixmap(data.primaryImageId, data.primaryImageTag);
        };
        if (!pix) pix = DummyImages::poster();
        m_cover->setPixmap(pix);
        m_actorModel->setStringList(toQStringList(data.actors));
        listViewSetVisibleRows(m_actorView, c_maxVisibleActors);
        m_directorModel->setStringList(toQStringList(data.directors));
        listViewSetVisibleRows(m_directorView, c_maxVisibleDirectors);
        m_studioModel->setStringList(toQStringList(data.studios));
        listViewSetVisibleRows(m_studioView, c_maxVisibleStudios);
        m_genreModel->setStringList(toQStringList(data.genres));
        listViewSetVisibleRows(m_genreView, c_maxVisibleGenres);
        m_overview->setText(toQString(data.overview));
    }

private:
    QLabel* m_title;
    QLabel* m_originalTitle;
    QLabel* m_cover;
    QStringListModel* m_actorModel;
    QStringListModel* m_directorModel;
    QStringListModel* m_studioModel;
    QStringListModel* m_genreModel;
    QListView* m_actorView;
    QListView* m_directorView;
    QListView* m_studioView;
    QListView* m_genreView;
    QTextBrowser* m_overview;
};

class SeriesDetailSheet : public QWidget {
    Q_OBJECT

public:
    explicit SeriesDetailSheet(QWidget* parent = nullptr) {
        auto* layout = new QVBoxLayout(this);
        m_title = makeTitleLabel(this);
        layout->addWidget(m_title);
        m_originalTitle = new QLabel(this);
        m_originalTitle->setWordWrap(true);
        layout->addWidget(m_originalTitle);
        m_cover = makeCoverLabel(this);
        layout->addWidget(m_cover);
        m_actorModel = new QStringListModel(this);
        m_actorView  = new QListView(this);
        m_actorView->setModel(m_actorModel);
        layout->addWidget(makeGroupWidget(c_txt_cast, m_actorView, this));
        m_studioModel = new QStringListModel(this);
        m_studioView  = new QListView(this);
        m_studioView->setModel(m_studioModel);
        layout->addWidget(makeGroupWidget(c_txt_studios, m_studioView, this));
        m_genreModel = new QStringListModel(this);
        m_genreView  = new QListView(this);
        m_genreView->setModel(m_genreModel);
        layout->addWidget(makeGroupWidget(c_txt_genres, m_genreView, this));
        m_overview = makeTextBrowser(this);
        layout->addWidget(makeGroupWidget(c_txt_overview, m_overview, this));
        layout->addStretch();
        finalizeSheet(this);
    }
    void setData(const SeriesDataInc& data, JBImageCache& cache) {
        QPixmap pix;
        m_title->setText(toQString(data.name));
        QString originalTitle = toQString(data.originalTitle);
        if (!originalTitle.isEmpty()) {
            m_originalTitle->setText(originalTitle);
            m_originalTitle->setVisible(true);
        } else m_originalTitle->setVisible(false);
        if (data.primaryImageTag != "") {
            pix = cache.getPixmap(data.primaryImageId, data.primaryImageTag);
        };
        if (!pix) pix = DummyImages::poster();
        m_cover->setPixmap(pix);
        m_actorModel->setStringList(toQStringList(data.actors));
        listViewSetVisibleRows(m_actorView, c_maxVisibleActors);
        m_studioModel->setStringList(toQStringList(data.studios));
        listViewSetVisibleRows(m_studioView, c_maxVisibleStudios);
        m_genreModel->setStringList(toQStringList(data.genres));
        listViewSetVisibleRows(m_genreView, c_maxVisibleGenres);
        m_overview->setText(toQString(data.overview));
    }

private:
    QLabel* m_title;
    QLabel* m_originalTitle;
    QLabel* m_cover;
    QStringListModel* m_actorModel;
    QStringListModel* m_studioModel;
    QStringListModel* m_genreModel;
    QListView* m_actorView;
    QListView* m_studioView;
    QListView* m_genreView;
    QTextBrowser* m_overview;
};

class SeasonDetailSheet : public QWidget {
    Q_OBJECT

public:
    explicit SeasonDetailSheet(QWidget* parent = nullptr) {
        auto* layout = new QVBoxLayout(this);
        m_title = makeTitleLabel(this);
        layout->addWidget(m_title);
        m_cover = makeCoverLabel(this);
        layout->addWidget(m_cover);
        m_episodeModel = new QStringListModel(this);
        m_episodeView  = new QListView(this);
        m_episodeView->setModel(m_episodeModel);
        layout->addWidget(makeGroupWidget(c_txt_episodes, m_episodeView, this));
        layout->addStretch();
        finalizeSheet(this);
    }
    void setData(const SeasonDataInc& data, const QStringList episodeNames, JBImageCache& cache) {
        QPixmap pix;
        m_title->setText(toQString(data.name));
        if (data.primaryImageTag != "") {
            pix = cache.getPixmap(data.primaryImageId, data.primaryImageTag);
        };
        if (!pix) pix = DummyImages::poster();
        m_cover->setPixmap(pix);
        m_episodeModel->setStringList(episodeNames);
        listViewSetVisibleRows(m_episodeView, c_maxVisibleEpisodes);
    }

private:
    QLabel* m_title;
    QLabel* m_cover;
    QStringListModel* m_episodeModel;
    QListView* m_episodeView;
};

class EpisodeDetailSheet : public QWidget {
    Q_OBJECT

public:
    explicit EpisodeDetailSheet(QWidget* parent = nullptr) {
        auto* layout = new QVBoxLayout(this);
        m_title = makeTitleLabel(this);
        layout->addWidget(m_title);
        m_originalTitle = new QLabel(this);
        m_originalTitle->setWordWrap(true);
        layout->addWidget(m_originalTitle);
        m_cover = makeCoverLabel(this);
        layout->addWidget(m_cover);
        m_actorModel = new QStringListModel(this);
        m_actorView  = new QListView(this);
        m_actorView->setModel(m_actorModel);
        layout->addWidget(makeGroupWidget(c_txt_cast, m_actorView, this));
        m_directorModel = new QStringListModel(this);
        m_directorView  = new QListView(this);
        m_directorView->setModel(m_directorModel);
        layout->addWidget(makeGroupWidget(c_txt_directors, m_directorView, this));
        m_overview = makeTextBrowser(this);
        layout->addWidget(makeGroupWidget(c_txt_overview, m_overview, this));
        layout->addStretch();
        finalizeSheet(this);
    }
    void setData(const EpisodeDataInc& data, JBImageCache& cache) {
        QPixmap pix;
        m_title->setText(toQString(data.name));
        QString originalTitle = toQString(data.originalTitle);
        if (!originalTitle.isEmpty()) {
            m_originalTitle->setText(originalTitle);
            m_originalTitle->setVisible(true);
        } else m_originalTitle->setVisible(false);
        if (data.primaryImageTag != "") {
            pix = cache.getPixmap(data.primaryImageId, data.primaryImageTag);
        };
        if (!pix) pix = DummyImages::frame();
        m_cover->setPixmap(pix);
        m_actorModel->setStringList(toQStringList(data.actors));
        listViewSetVisibleRows(m_actorView, c_maxVisibleActors);
        m_directorModel->setStringList(toQStringList(data.directors));
        listViewSetVisibleRows(m_directorView, c_maxVisibleDirectors);
        m_overview->setText(toQString(data.overview));
    }

private:
    QLabel* m_title;
    QLabel* m_originalTitle;
    QLabel* m_cover;
    QStringListModel* m_actorModel;
    QStringListModel* m_directorModel;
    QListView* m_actorView;
    QListView* m_directorView;
    QTextBrowser* m_overview;
};

class HomeVideoDetailSheet : public QWidget {
    Q_OBJECT

public:
    explicit HomeVideoDetailSheet(QWidget* parent = nullptr) {
        auto* layout = new QVBoxLayout(this);
        m_title = makeTitleLabel(this);
        layout->addWidget(m_title);
        m_cover = makeCoverLabel(this);
        layout->addWidget(m_cover);
        layout->setAlignment(m_cover, Qt::AlignTop);
        m_peopleModel = new QStringListModel(this);
        m_peopleView  = new QListView(this);
        m_peopleView->setModel(m_peopleModel);
        layout->addWidget(makeGroupWidget(c_txt_people, m_peopleView, this));
        m_genreModel = new QStringListModel(this);
        m_genreView  = new QListView(this);
        m_genreView->setModel(m_genreModel);
        layout->addWidget(makeGroupWidget(c_txt_genres, m_genreView, this));
        m_overview = makeTextBrowser(this);
        layout->addWidget(makeGroupWidget(c_txt_overview, m_overview, this));
        layout->addStretch();
        finalizeSheet(this);
    }
    void setData(const HomeVideoDataInc& data, JBImageCache& cache) {
        QPixmap pix;
        m_title->setText(toQString(data.name));
        if (data.primaryImageTag != "") {
            pix = cache.getPixmap(data.primaryImageId, data.primaryImageTag);
        };
        if (!pix) pix = DummyImages::frame();
        m_cover->setPixmap(pix);
        m_peopleModel->setStringList(toQStringList(data.people));
        m_genreModel->setStringList(toQStringList(data.genres));
        listViewSetVisibleRows(m_genreView, c_maxVisibleGenres);
        m_overview->setText(toQString(data.overview));
    }

private:
    QLabel* m_title;
    QLabel* m_cover;
    QStringListModel* m_peopleModel;
    QStringListModel* m_genreModel;
    QListView* m_peopleView;
    QListView* m_genreView;
    QTextBrowser* m_overview;
};

class MusicVideoDetailSheet : public QWidget {
    Q_OBJECT

public:
    explicit MusicVideoDetailSheet(QWidget* parent = nullptr) {
        auto* layout = new QVBoxLayout(this);
        m_title = makeTitleLabel(this);
        layout->addWidget(m_title);
        m_cover = makeCoverLabel(this);
        layout->addWidget(m_cover);
        m_artistModel = new QStringListModel(this);
        m_artistView  = new QListView(this);
        m_artistView->setModel(m_artistModel);
        layout->addWidget(makeGroupWidget(c_txt_artists, m_artistView, this));
        m_genreModel = new QStringListModel(this);
        m_genreView  = new QListView(this);
        m_genreView->setModel(m_genreModel);
        layout->addWidget(makeGroupWidget(c_txt_genres, m_genreView, this));
        m_overview = makeTextBrowser(this);
        layout->addWidget(makeGroupWidget(c_txt_overview, m_overview, this));
        layout->addStretch();
        finalizeSheet(this);
    }
    void setData(const MusicVideoDataInc& data, JBImageCache& cache) {
        QPixmap pix;
        m_title->setText(toQString(data.name));
        if (data.primaryImageTag != "") {
            pix = cache.getPixmap(data.primaryImageId, data.primaryImageTag);
        };
        if (!pix) pix = DummyImages::frame();
        m_cover->setPixmap(pix);
        m_artistModel->setStringList(toQStringList(data.artists));
        listViewSetVisibleRows(m_artistView, c_maxVisibleArtists);
        m_genreModel->setStringList(toQStringList(data.genres));
        listViewSetVisibleRows(m_genreView, c_maxVisibleGenres);
        m_overview->setText(toQString(data.overview));
    }

private:
    QLabel* m_title;
    QLabel* m_cover;
    QStringListModel* m_artistModel;
    QListView* m_artistView;
    QStringListModel* m_genreModel;
    QListView* m_genreView;
    QTextBrowser* m_overview;
};

class AlbumDetailSheet : public QWidget {
    Q_OBJECT

public:
    explicit AlbumDetailSheet(QWidget* parent = nullptr) {
        auto* layout = new QVBoxLayout(this);
        m_title = makeTitleLabel(this);
        layout->addWidget(m_title);
        m_artist = new QLabel(this);
        layout->addWidget(m_artist);
        m_cover = makeCoverLabel(this);
        layout->addWidget(m_cover);
        m_artistModel = new QStringListModel(this);
        m_artistView  = new QListView(this);
        m_artistView->setModel(m_artistModel);
        layout->addWidget(makeGroupWidget(c_txt_artists, m_artistView, this));
        m_genreModel = new QStringListModel(this);
        m_genreView  = new QListView(this);
        m_genreView->setModel(m_genreModel);
        layout->addWidget(makeGroupWidget(c_txt_genres, m_genreView, this));
        m_trackModel = new QStringListModel(this);
        m_trackView  = new QListView(this);
        m_trackView->setModel(m_trackModel);
        layout->addWidget(makeGroupWidget(c_txt_tracks, m_trackView, this));
        layout->addStretch();
        finalizeSheet(this);
    }
    void setData(const AlbumDataInc& data, QStringList trackNames, JBImageCache& cache) {
        QPixmap pix;
        m_title->setText(toQString(data.name));
        if (data.primaryImageTag != "") {
            pix = cache.getPixmap(data.primaryImageId, data.primaryImageTag);
        };
        if (!pix) pix = DummyImages::square();
        m_cover->setPixmap(pix);
        m_artistModel->setStringList(toQStringList(data.artists));
        listViewSetVisibleRows(m_artistView, c_maxVisibleArtists);
        m_genreModel->setStringList(toQStringList(data.genres));
        listViewSetVisibleRows(m_genreView, c_maxVisibleGenres);
        m_trackModel->setStringList(trackNames);
        listViewSetVisibleRows(m_trackView, c_maxVisibleTracks);
    }

private:
    QLabel* m_title;
    QLabel* m_artist;
    QLabel* m_cover;
    QStringListModel* m_artistModel;
    QStringListModel* m_genreModel;
    QStringListModel* m_trackModel;
    QListView* m_artistView;
    QListView* m_genreView;
    QListView* m_trackView;
};

class TrackDetailSheet : public QWidget {
    Q_OBJECT

public:
    explicit TrackDetailSheet(QWidget* parent = nullptr) {
        auto* layout = new QVBoxLayout(this);
        m_title = makeTitleLabel(this);
        layout->addWidget(m_title);
        m_artist = new QLabel(this);
        layout->addWidget(m_artist);
        m_cover = makeCoverLabel(this);
        layout->addWidget(m_cover);
        m_artistModel = new QStringListModel(this);
        m_artistView  = new QListView(this);
        m_artistView->setModel(m_artistModel);
        layout->addWidget(makeGroupWidget(c_txt_artists, m_artistView, this));
        m_genreModel = new QStringListModel(this);
        m_genreView  = new QListView(this);
        m_genreView->setModel(m_genreModel);
        layout->addWidget(makeGroupWidget(c_txt_genres, m_genreView, this));
        layout->addStretch();
        finalizeSheet(this);
    }
    void setData(const AudioDataInc& data, JBImageCache& cache) {
        QPixmap pix;
        m_title->setText(toQString(data.name));
        if (data.primaryImageTag != "") {
            pix = cache.getPixmap(data.primaryImageId, data.primaryImageTag);
        };
        if (!pix) pix = DummyImages::square();
        m_cover->setPixmap(pix);
        m_artistModel->setStringList(toQStringList(data.artists));
        listViewSetVisibleRows(m_artistView, c_maxVisibleArtists);
        m_genreModel->setStringList(toQStringList(data.genres));
        listViewSetVisibleRows(m_genreView, c_maxVisibleGenres);
    }

private:
    QLabel* m_title;
    QLabel* m_artist;
    QLabel* m_cover;
    QStringListModel* m_artistModel;
    QStringListModel* m_genreModel;
    QListView* m_artistView;
    QListView* m_genreView;
};
