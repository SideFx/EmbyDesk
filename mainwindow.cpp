/////////////////////////////////////////////////////////////////////////////
// Name:        mainwindow.cpp
// Purpose:     The main window
// Author:      Jan Buchholz
// Created:     2026-04-23
// Changed:     2026-05-18
/////////////////////////////////////////////////////////////////////////////

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stylesheets.h"
#include "preferencesdialog.h"
#include "jbpreferences.h"
#include "conversions.hpp"
#include "apicall.h"
#include "aboutdialog.h"
#include "embydumpdialog.h"
#include <QHeaderView>
#include <QTimer>
#include <QFileDialog>
#include <QStandardPaths>
#include "excelexport.h"
#include "appsettings.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle(QString(APP_NAME) + " v" + APP_VERSION);
    setMinimumSize(DEF_WINDOW_MINSIZE);
    createToolBar();
    createMainControls();
    createStatusBar();
    setCentralWidget(m_mainSplitter);
    m_offlineMode = false;
    // --satisfy macOS---
    APICall::sendNetworkBroadcast();
    loadSettings();
    showSplashScreen();
    setConnections();
    loadSettings();
    setProxyModels();
    enableFunctions();
}

MainWindow::~MainWindow() {
    delete ui;
    m_sqlReader.closeDBConnection();
}

void MainWindow::createToolBar() {
    m_mainToolBar = new QToolBar(this);
    m_mainToolBar->setObjectName("mainToolBar");
    m_mainToolBar->setMovable(false);
    m_mainToolBar->setOrientation(Qt::Horizontal);
    m_mainToolBar->setAutoFillBackground(true);
    m_mainToolBar->setIconSize(QSize(DEF_ICONSIZE, DEF_ICONSIZE));
    m_mainToolBar->setFixedHeight(DEF_TOOLBAR_HEIGHT);
#if defined(Q_OS_WIN)
    m_mainToolBar->setStyleSheet(styleToolBar);
#elif defined(Q_OS_MAC)
    m_mainToolBar->setStyleSheet(styleToolButton);
#endif
    m_mainToolBar->addAction(ui->actionSettings);
    m_mainToolBar->addAction(ui->actionLogin);
    m_mainToolBar->addSeparator();
    m_collectionBox = new QComboBox;
    m_collectionBox->setMinimumWidth(180);
    m_mainToolBar->addWidget(m_collectionBox);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(ui->actionRetrieve);
    m_mainToolBar->addAction(ui->actionExport);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction((ui->actionDump));
    m_mainToolBar->addAction(ui->actionOffline);
    QWidget* spacerSmall = new QWidget;
    spacerSmall->setMinimumWidth(10);
    m_mainToolBar->addWidget(spacerSmall);
    m_stats = new QLabel;
    m_stats->setMinimumWidth(180);
    m_mainToolBar->addWidget(m_stats);
    QWidget* spacerLarge = new QWidget;
    spacerLarge->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_mainToolBar->addWidget(spacerLarge);
    m_mainToolBar->addAction(ui->actionAbout);
    m_mainToolBar->addAction(ui->actionQuit);
    this->addToolBar(Qt::TopToolBarArea, m_mainToolBar);
}

void MainWindow::createMainControls() {
    m_mainSplitter = new QSplitter(this);
    m_mainSplitter->setObjectName("mainSplitter");
    m_mainSplitter->setOrientation(Qt::Horizontal);
#if defined(Q_OS_MAC)
    m_mainSplitter->setStyleSheet(styleSplitterHandle);
#endif
    m_tree = new QTreeView(m_mainSplitter);
    m_tree->setAlternatingRowColors(true);
    m_tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_sheets = new QStackedWidget(m_mainSplitter);
    // ---stack sheets---
    m_movieSheet = new MovieDetailSheet(this);
    m_sheets->addWidget(m_movieSheet);
    m_seriesSheet = new SeriesDetailSheet(m_sheets);
    m_sheets->addWidget(m_seriesSheet);
    m_seasonSheet = new SeasonDetailSheet(m_sheets);
    m_sheets->addWidget(m_seasonSheet);
    m_episodeSheet = new EpisodeDetailSheet(m_sheets);
    m_sheets->addWidget(m_episodeSheet);
    m_homeVideoSheet = new HomeVideoDetailSheet(m_sheets);
    m_sheets->addWidget(m_homeVideoSheet);
    m_musicVideoSheet = new MusicVideoDetailSheet(m_sheets);
    m_sheets->addWidget(m_musicVideoSheet);
    m_albumSheet = new AlbumDetailSheet(m_sheets);
    m_sheets->addWidget(m_albumSheet);
    m_trackSheet = new TrackDetailSheet(m_sheets);
    m_sheets->addWidget(m_trackSheet);
    m_mainSplitter->addWidget(m_tree);
    m_mainSplitter->addWidget(m_sheets);
    m_sheets->setFixedWidth(DEF_SHEET_WIDTH);
    m_mainSplitter->setHandleWidth(0);
    m_mainSplitter->setChildrenCollapsible(false);
    m_mainSplitter->setVisible(false);
}

void MainWindow::createStatusBar() {
    m_statusBar = new QStatusBar(this);
#if defined(Q_OS_MAC)
    m_statusBar->setStyleSheet("QStatusBar {background: lightGray;}");
#endif
    auto* container = new QWidget(this);
    auto* layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    m_statusIcon = new QLabel(container);
    m_statusText = new QLabel(container);
    m_statusText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(m_statusIcon);
    layout->addWidget(m_statusText);
    this->setStatusBar(m_statusBar);
    m_statusBar->addPermanentWidget(container, 1);
    m_statusBar->layout()->activate();
    m_statusTimer = new QTimer(this);
    m_statusTimer->setSingleShot(true);
    connect(m_statusTimer, &QTimer::timeout, this, [this]() {
        showMessage(clear, "");
    });
}

void MainWindow::setConnections() {
    // ---Toolbar Buttons---
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::onActionQuit);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::onActionSettings);
    connect(ui->actionLogin, &QAction::triggered, this, &MainWindow::onActionLogin);
    connect(ui->actionRetrieve, &QAction::triggered, this, &MainWindow::onActionRetrieve);
    connect(ui->actionExport, &QAction::triggered, this, &MainWindow::onActionExport);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onActionAbout);
    connect(ui->actionDump, &QAction::triggered, this, &MainWindow::onActionDump);
    connect(ui->actionOffline, &QAction::triggered, this, &MainWindow::onActionOffline);
    // ---Collections ComboBox---
    connect(m_collectionBox, &QComboBox::currentIndexChanged, this, &MainWindow::onCollectionChanged);
    // ---Tree, automagically select row on expand/collapse---
    auto selectRow = [this](const QModelIndex& idx) {
        m_tree->selectionModel()->setCurrentIndex(
            idx,
            QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows
        );
    };
    connect(m_tree, &QTreeView::expanded,  this, selectRow);
    connect(m_tree, &QTreeView::collapsed, this, selectRow);
}

void MainWindow::setProxyModels() {
    // ---set sort proxies for the non-hierarchical models---
    m_movieProxy = new QSortFilterProxyModel(this);
    m_movieProxy->setSourceModel(&m_movieModel);
    m_movieProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_movieProxy->setSortRole(MovieTreeModel::SortRole);
    m_movieProxy->setDynamicSortFilter(true);

    m_homeVideoProxy = new QSortFilterProxyModel(this);
    m_homeVideoProxy->setSourceModel(&m_homeVideoModel);
    m_homeVideoProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_homeVideoProxy->setSortRole(HomeVideoTreeModel::SortRole);
    m_homeVideoProxy->setDynamicSortFilter(true);

    m_musicVideoProxy = new QSortFilterProxyModel(this);
    m_musicVideoProxy->setSourceModel(&m_musicVideoModel);
    m_musicVideoProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_musicVideoProxy->setSortRole(MusicVideoTreeModel::SortRole);
    m_musicVideoProxy->setDynamicSortFilter(true);
}

void MainWindow::onActionSettings() {
    PreferencesDialog p;
    p.setSettings(AppSettings::getSettings());
    if (p.exec() == QDialog::Accepted) {
        AppSettings::setSettings(p.getSettings());
        enableFunctions();
    };
}

void MainWindow::onActionLogin() {
    AppSettings::AppPreferences settings = AppSettings::getSettings();
    APICall::ApiResult result = APICall::userLoginToServer(settings.secure,
                                                           settings.host,
                                                           settings.port,
                                                           settings.userName,
                                                           settings.userPw);
    if (result.code == 0) {
        UserViewsResult views = APICall::userGetViews();
        if (views.code == 0) {
            m_collectionBox->clear();
            for (auto& v : views.views) {
                EmbyCollection e = {
                    .name = toQString(v.name),
                    .type = toQString(v.collectionType),
                    .id = toQString(v.id),
                };
                QIcon icon = getIconForCollectionType(v.collectionType);
                m_collectionBox->addItem(icon, e.name, QVariant::fromValue(e));
            }
            enableFunctions();
        } else {
            showMessage(error, toQString(views.message), DEF_MSG_DURATION);
        }
    } else {
        showMessage(error, toQString(result.message), DEF_MSG_DURATION);
    }
}

void MainWindow::onActionRetrieve() {
    m_itemCount = 0;
    int cols = -1;
    if (!APICall::getLoginResult() && !m_offlineMode) return;
    std::string id = toStandardString(m_collection.id);
    // ---Movies---
    if (m_collection.type == CollectionMovies) {
        m_offlineMode == false
            ? m_movies = APICall::userGetMovieData(id)
            : m_movies = m_sqlReader.loadMovies(m_collection.id);
        if (m_movies.code == 0) {
            m_itemCount = m_movies.movies.tMovieData.size();
            if (m_itemCount > 0) {
                m_tree->setRootIsDecorated(false);
                m_movieModel.setData(m_movies.movies);
                m_tree->setModel(m_movieProxy);
                m_tree->setSortingEnabled(true);
                m_tree->header()->setSortIndicatorShown(true);
                m_tree->header()->setSectionsClickable(true);
                if (m_movieModel.rowCount() > 0) {
                    m_movieProxy->sort(MovieTreeModel::Name, Qt::AscendingOrder);
                    m_tree->header()->setSortIndicator(MovieTreeModel::Name, Qt::AscendingOrder);
                }
                cols = m_movieModel.columnCount();
                m_stats->setText(m_movieModel.getStatistics());
            }
        } else {
            showMessage(error, toQString(m_movies.message));
            return;
        }
    }
    // ---Series---
    else if (m_collection.type == CollectionSeries) {
        m_offlineMode == false
            ? m_series = APICall::userGetSeriesData(id)
            : m_series = m_sqlReader.loadSeries(m_collection.id);
        if (m_series.code == 0) {
            m_itemCount = m_series.series.tSeriesData.size();
            if (m_itemCount > 0) {
                m_tree->setRootIsDecorated(true);
                m_seriesModel.setData(m_series.series);
                m_tree->setModel(&m_seriesModel);
                m_tree->setSortingEnabled(false);
                m_tree->header()->setSortIndicatorShown(false);
                m_tree->header()->setSectionsClickable(false);
                cols = m_seriesModel.columnCount();
                m_stats->setText(m_seriesModel.getStatistics());
            }
        } else {
            showMessage(error, toQString(m_series.message));
            return;
        }
    }
    // ---Home Videos---
    else if (m_collection.type == CollectionHomeVideos) {
        m_offlineMode == false
            ? m_homeVideos = APICall::userGetHomeVideoData(id)
            : m_homeVideos = m_sqlReader.loadHomeVideos(m_collection.id);
        if (m_homeVideos.code == 0) {
            m_itemCount = m_homeVideos.homeVideos.tHomeVideoData.size();
            if (m_itemCount > 0) {
                m_tree->setRootIsDecorated(false);
                m_homeVideoModel.setData(m_homeVideos.homeVideos);
                m_tree->setModel(m_homeVideoProxy);
                m_tree->setSortingEnabled(true);
                m_tree->header()->setSortIndicatorShown(true);
                m_tree->header()->setSectionsClickable(true);
                if (m_homeVideoModel.rowCount() > 0) {
                    m_homeVideoProxy->sort(HomeVideoTreeModel::Name, Qt::AscendingOrder);
                    m_tree->header()->setSortIndicator(HomeVideoTreeModel::Name, Qt::AscendingOrder);
                }
                cols = m_homeVideoModel.columnCount();
                m_stats->setText(m_homeVideoModel.getStatistics());
            }
        } else {
            showMessage(error, toQString(m_homeVideos.message));
            return;
        }
    }
    // ---Music Videos---
    else if (m_collection.type == CollectionMusicVideos) {
        m_offlineMode == false
            ? m_musicVideos = APICall::userGetMusicVideoData(id)
            : m_musicVideos = m_sqlReader.loadMusicVideos(m_collection.id);
        if (m_musicVideos.code == 0) {
            m_itemCount = m_musicVideos.musicVideos.tMusicVideoData.size();
            if (m_itemCount > 0) {
                m_tree->setRootIsDecorated(false);
                m_musicVideoModel.setData(m_musicVideos.musicVideos);
                m_tree->setModel(m_musicVideoProxy);
                m_tree->setSortingEnabled(true);
                m_tree->header()->setSortIndicatorShown(true);
                m_tree->header()->setSectionsClickable(true);
                if (m_musicVideoModel.rowCount() > 0) {
                    m_musicVideoProxy->sort(MusicVideoTreeModel::Name, Qt::AscendingOrder);
                    m_tree->header()->setSortIndicator(MusicVideoTreeModel::Name, Qt::AscendingOrder);
                }
                cols = m_musicVideoModel.columnCount();
                m_stats->setText(m_musicVideoModel.getStatistics());
            }
        } else {
            showMessage(error, toQString(m_musicVideos.message));
            return;
        }
    }
    // ---Music---
    else if (m_collection.type == CollectionMusic) {
        m_offlineMode == false
            ? m_music = APICall::userGetMusicData(id)
            : m_music = m_sqlReader.loadMusic(m_collection.id);
        if (m_music.code == 0) {
            m_itemCount = m_music.music.tAlbumData.size();
            if (m_itemCount > 0) {
                m_tree->setRootIsDecorated(true);
                m_musicModel.setData(m_music.music);
                m_tree->setModel(&m_musicModel);
                m_tree->setSortingEnabled(false);
                m_tree->header()->setSortIndicatorShown(false);
                m_tree->header()->setSectionsClickable(false);
                cols = m_musicModel.columnCount();
                m_stats->setText(m_musicModel.getStatistics());
            }
        } else {
            showMessage(error, toQString(m_music.message));
            return;
        }
    } else return;
    if (m_itemCount > 0) {
        auto sel = m_tree->selectionModel();
        if (sel) {
            connect(sel, &QItemSelectionModel::currentChanged, this, &MainWindow::onCurrentChanged);
            QTimer::singleShot(0, this, [this, sel]() {
                QAbstractItemModel *model = m_tree->model();
                QModelIndex first = model->index(0, 0);
                if (first.isValid()) {
                    sel->setCurrentIndex(first,
                                         QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
                    m_tree->scrollTo(first);
                }
            });
        }
        m_tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        m_tree->header()->setStretchLastSection(true);
        for (int c = 0; c < cols; ++c) m_tree->resizeColumnToContents(c);
        m_splashDialog.hide();
        m_mainSplitter->setVisible(true);
        ui->actionExport->setEnabled(true);
    }
    m_tree->setFocus();
}

void MainWindow::onActionExport() {
    QFileDialog saveDialog;
    const auto now = QDateTime::currentDateTime();
    QString preferredFileName = QString(DEF_EXPORT_PREFIX) + "_" + m_collection.name + "_" +
                                now.date().toString("yyyyMMdd") +
                                now.time().toString("hhmmss") +
                                QString(DEF_EXPORT_SUFFIX);
    QString folder = (m_lastFolderXlsx.isEmpty())
                     ? QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                     : m_lastFolderXlsx;
    saveDialog.setParent(this, Qt::Dialog |
                               Qt::WindowSystemMenuHint |
                               Qt::WindowCloseButtonHint |
                               Qt::WindowTitleHint |
                               Qt::CustomizeWindowHint);
    saveDialog.setOption(QFileDialog::DontUseNativeDialog, true);
    saveDialog.setDirectory(folder);
    saveDialog.setAcceptMode(QFileDialog::AcceptSave);
    saveDialog.setFileMode(QFileDialog::AnyFile);
    saveDialog.setNameFilter(DEF_XLSX_FILE_FILTER);
    saveDialog.setViewMode(QFileDialog::Detail);
    saveDialog.selectFile(preferredFileName);
    if (saveDialog.exec() != QDialog::Accepted) return;
    QStringList files = saveDialog.selectedFiles();
    QString fileName = files.first();
    QFileInfo fileInfo(fileName);
    m_lastFolderXlsx = fileInfo.path();
    QAbstractItemModel* model;
    if (m_collection.type == CollectionMovies) model = &m_movieModel;
    else if (m_collection.type == CollectionSeries) model = &m_seriesModel;
    else if (m_collection.type == CollectionHomeVideos) model = &m_homeVideoModel;
    else if (m_collection.type == CollectionMusicVideos) model = &m_musicVideoModel;
    else if (m_collection.type == CollectionMusic) model = &m_musicModel;
    else return;
    ExcelExport exp;
    if (exp.doExport(model, m_collection, fileName)) {
        showMessage(information, tr("Collection exported to file ") + fileInfo.fileName(), DEF_MSG_DURATION);
    } else {
        showMessage(error, ERROR_MESSAGES[MSG_XLSX_WRITE_ERROR], DEF_MSG_DURATION);
    }
}

void MainWindow::onActionDump() {
    ui->actionOffline->setEnabled(false);
    connect(&m_dumpDialog,
            &EmbyDumpDialog::sendPathInfo,
            this,
            &MainWindow::onReceivePathInfo,
            Qt::UniqueConnection);
    m_dumpDialog.setSQLiteDirectory(m_lastFolderDb);
    m_dumpDialog.exec();
    ui->actionOffline->setEnabled(true);
}

// ---Update path & dbname for App preferences---
void MainWindow::onReceivePathInfo(const QString& directory, const QString& name) {
    m_lastFolderDb = directory;
    m_lastUsedDb = name;
}

void MainWindow::onActionOffline(){
    if (m_offlineMode) {
        resetModelsAndUI();
        m_offlineMode = false;
        showMessage(information,
                    tr("Offline mode has finished. You may log in to your Emby server again."),
                    DEF_MSG_DURATION);
        return;
    }
    m_offlineMode = false;
    if (ui->actionOffline->isChecked()) {
        QFileDialog openDialog;
        QString fileName = "";
        QString folder = (m_lastFolderDb.isEmpty())
                         ? QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                         : m_lastFolderXlsx;
        openDialog.setParent(this, Qt::Dialog |
                                   Qt::WindowSystemMenuHint |
                                   Qt::WindowCloseButtonHint |
                                   Qt::WindowTitleHint |
                                   Qt::CustomizeWindowHint);
        openDialog.setDirectory(folder);
        openDialog.setAcceptMode(QFileDialog::AcceptOpen);
        openDialog.setFileMode(QFileDialog::ExistingFile);
        openDialog.setNameFilter(DEF_SQLITE_FILE_FILTER);
        openDialog.setOptions(QFileDialog::DontUseNativeDialog | QFileDialog::ReadOnly);
        openDialog.setViewMode(QFileDialog::Detail);
        if (openDialog.exec() == QDialog::Accepted) {
            QStringList files = openDialog.selectedFiles();
            if (!files.empty()) {
                fileName = files.first();
                ErrorCode ec = m_sqlReader.openAndCheckDB(fileName);
                if (ec != MSG_OK) {
                    showMessage(error, ERROR_MESSAGES[ec], DEF_MSG_DURATION);
                    ui->actionOffline->setChecked(false);
                    return;
                }
                EmbyCollectionResult ecr = m_sqlReader.loadCollections();
                if (ecr.code != MSG_OK) {
                    showMessage(error, ecr.message);
                    ui->actionOffline->setChecked(false);
                    return;
                }
                resetModelsAndUI();
                for (auto& c : ecr.collections) {
                    QIcon icon = getIconForCollectionType(c.type.toStdString());
                    m_collectionBox->addItem(icon, c.name, QVariant::fromValue(c));
                }
                m_offlineMode = true;
                ui->actionRetrieve->setEnabled(true);
                return;
            }
        }
    }
    ui->actionOffline->setChecked(false);
}

void MainWindow::onActionAbout() {
    AboutDialog dlg;
    dlg.setParent(this);
    int pw = width();
    int ph = height() + m_mainToolBar->height();
    int mw = dlg.width();
    int mh = dlg.height();
    if (pw >= mw && ph >= mh) {
        dlg.move((pw - mw) / 2, (ph - mh) / 2);
    }
    dlg.exec();
}

void MainWindow::onActionQuit() {
    close();
}

void MainWindow::onCollectionChanged(int index) {
    if (index < 0) return;
    m_splashDialog.show();
    m_mainSplitter->setVisible(false);
    m_itemCount = 0;
    m_stats->setText("");
    m_collection = m_collectionBox->itemData(index).value<EmbyCollection>();
    ui->actionExport->setEnabled(false);
}

void MainWindow::onCurrentChanged(const QModelIndex& current, const QModelIndex& previous) {
    if (!current.isValid()) return;
    const QAbstractItemModel *model = current.model();
    // ---check, if model is a proxy---
    if (auto *proxy = qobject_cast<const QAbstractProxyModel*>(model)) {
        QModelIndex source = proxy->mapToSource(current);
        loadSheetForItem(source);
        return;
    }
    loadSheetForItem(current);
}

void MainWindow::resizeEvent(QResizeEvent *e) {
    if (m_splashDialog.isVisible()) centerSplashDialog(m_splashDialog);
}

void MainWindow::closeEvent(QCloseEvent *e) {
    saveSettings();
    e->accept();
}

void MainWindow::enableFunctions() {
    AppSettings::AppPreferences settings = AppSettings::getSettings();
    bool b = !settings.host.isEmpty() &&
             !settings.port.isEmpty() &&
             !settings.userName.isEmpty() &&
             !settings.userPw.isEmpty();
    ui->actionLogin->setEnabled(b);
    bool c = m_collectionBox->count() > 0;
    ui->actionRetrieve->setEnabled(c);
    ui->actionDump->setEnabled(c);
    ui->actionOffline->setEnabled(true);
    ui->actionExport->setEnabled(m_itemCount > 0);
}

void MainWindow::saveSettings() {
    JBPreferences prefs{};
    AppSettings::AppPreferences settings = AppSettings::getSettings();
    prefs.PushArray(SET_WINDOW_GEOMETRY, saveGeometry());
    prefs.PushArray(SET_WINDOW_STATE, saveState(0));
    prefs.PushBoolean(SET_EMBY_SECURE, settings.secure);
    prefs.PushString(SET_EMBY_ADDRESS, settings.host);
    prefs.PushString(SET_EMBY_PORT, settings.port);
    prefs.PushString(SET_EMBY_USERNAME, settings.userName);
    prefs.PushString(SET_EMBY_PASSWORD, settings.userPw);
    prefs.PushString(SET_FOLDER_XLSX, m_lastFolderXlsx);
    prefs.PushString(SET_FOLDER_DB, m_lastFolderDb);
    prefs.PushString(SET_LAST_USED_DB, m_lastUsedDb);
    prefs.PushNumber(SET_MAX_ACTORS, settings.max_actors);
    prefs.PushNumber(SET_MAX_DIRECTORS, settings.max_directors);
    prefs.PushNumber(SET_MAX_GENRES, settings.max_genres);
    prefs.PushNumber(SET_MAX_STUDIOS, settings.max_studios);
    prefs.PushBoolean(SET_LIM_ACTORS, settings.limit_actors);
    prefs.PushBoolean(SET_LIM_DIRECTORS, settings.limit_directors);
    prefs.PushBoolean(SET_LIM_GENRES, settings.limit_genres);
    prefs.PushBoolean(SET_LIM_STUDIOS, settings.limit_studios);
    prefs.SavePreferencesToDefaultLocation(APP_COMPANY, APP_NAME);
}

void MainWindow::loadSettings() {
    JBPreferences prefs;
    AppSettings::AppPreferences settings;
    if (prefs.LoadPreferencesFromDefaultLocation(APP_COMPANY, APP_NAME)) {
        restoreGeometry(prefs.PopArray(SET_WINDOW_GEOMETRY));
        restoreState(prefs.PopArray(SET_WINDOW_STATE));
        settings = {
            .secure = prefs.PopBoolean(SET_EMBY_SECURE),
            .host = prefs.PopString(SET_EMBY_ADDRESS),
            .port = prefs.PopString(SET_EMBY_PORT),
            .userName = prefs.PopString(SET_EMBY_USERNAME),
            .userPw = prefs.PopString(SET_EMBY_PASSWORD),
            .max_actors = static_cast<int>(prefs.PopNumber(SET_MAX_ACTORS)),
            .max_directors = static_cast<int>(prefs.PopNumber(SET_MAX_DIRECTORS)),
            .max_studios = static_cast<int>(prefs.PopNumber(SET_MAX_STUDIOS)),
            .max_genres = static_cast<int>(prefs.PopNumber(SET_MAX_GENRES)),
            .limit_actors = prefs.PopBoolean(SET_LIM_ACTORS),
            .limit_directors = prefs.PopBoolean(SET_LIM_DIRECTORS),
            .limit_studios = prefs.PopBoolean(SET_LIM_STUDIOS),
            .limit_genres = prefs.PopBoolean(SET_LIM_STUDIOS),
        };
        m_lastFolderXlsx = prefs.PopString(SET_FOLDER_XLSX);
        m_lastFolderDb = prefs.PopString(SET_FOLDER_DB);
        m_lastUsedDb = prefs.PopString(SET_LAST_USED_DB);
        AppSettings::setSettings(settings);
    }
}

QIcon MainWindow::getIconForCollectionType(const std::string& type) {
    if (type == CollectionMovies) return QIcon(":/coll/assets/type_Movie.svg");
    if (type == CollectionSeries) return QIcon(":/coll/assets/type_Series.svg");
    if (type == CollectionHomeVideos) return QIcon(":/coll/assets/type_HomeVideo.svg");
    if (type == CollectionMusicVideos) return QIcon(":/coll/assets/type_MusicVideo.svg");
    if (type == CollectionMusic) return QIcon(":/coll/assets/type_Music.svg");
    return QIcon(":/coll/assets/type_Folder.svg"); // fallback
}

void MainWindow::showSplashScreen() {
    m_splashDialog.setParent(this);
    centerSplashDialog(m_splashDialog);
    m_splashDialog.showNormal();
}

void MainWindow::centerSplashDialog(SplashDialog& dlg) {
    int pw = width();
    int ph = height() + m_mainToolBar->height();
    int mw = dlg.width();
    int mh = dlg.height();
    if (pw >= mw && ph >= mh) {
        dlg.move((pw - mw) / 2, (ph - mh) / 2);
    }
}

void MainWindow::loadSheetForItem(const QModelIndex &sourceIndex) {
    const QAbstractItemModel *model = sourceIndex.model();
    // ---Load sheet for Movies---
    if (auto movieModel = qobject_cast<const MovieTreeModel*>(model)) {
        MovieTreeModel::Node* node = movieModel->itemAt(sourceIndex);
        if (node->type == MovieTreeModel::Node::Type::Movie) {
            auto* data = movieModel->movieData(node);
            m_movieSheet->setData(*data);
            m_sheets->setCurrentWidget(m_movieSheet);
        }
        return;
    }
    // ---Load sheets for Series, Seasons & Episodes---
    if (auto seriesModel = qobject_cast<const SeriesTreeModel*>(model)) {
        SeriesTreeModel::Node* node = seriesModel->itemAt(sourceIndex);
        switch (node->type) {
            case SeriesTreeModel::Node::Type::Series: {
                auto* data = seriesModel->seriesData(node);
                m_seriesSheet->setData(*data);
                m_sheets->setCurrentWidget(m_seriesSheet);
                break;
            }
            case SeriesTreeModel::Node::Type::Season:  {
                auto* data = seriesModel->seasonData(node);
                auto episodeNames = seriesModel->episodeNamesForSeason(node);
                m_seasonSheet->setData(*data, episodeNames);
                m_sheets->setCurrentWidget(m_seasonSheet);
                break;
            }
            case SeriesTreeModel::Node::Type::Episode: {
                auto* data = seriesModel->episodeData(node);
                m_episodeSheet->setData(*data);
                m_sheets->setCurrentWidget(m_episodeSheet);
                break;
            }
        }
        return;
    }
    // ---Load sheet for HomeVideos---
    if (auto homeVideoModel = qobject_cast<const HomeVideoTreeModel*>(model)) {
        HomeVideoTreeModel::Node* node = homeVideoModel->itemAt(sourceIndex);
        if (node->type == HomeVideoTreeModel::Node::Type::HomeVideo) {
            auto* data = homeVideoModel->homeVideoData(node);
            m_homeVideoSheet->setData(*data);
            m_sheets->setCurrentWidget(m_homeVideoSheet);
        }
        return;
    }
    // ---Load sheet for MusicVideos---
    if (auto musicVideoModel = qobject_cast<const MusicVideoTreeModel*>(model)) {
        MusicVideoTreeModel::Node* node = musicVideoModel->itemAt(sourceIndex);
        if (node->type == MusicVideoTreeModel::Node::Type::MusicVideo) {
            auto* data = musicVideoModel->musicVideoData(node);
            m_musicVideoSheet->setData(*data);
            m_sheets->setCurrentWidget(m_musicVideoSheet);
        }
        return;
    }
    // ---Load sheets for Music---
    if (auto musicModel = qobject_cast<const MusicTreeModel*>(model)) {
        MusicTreeModel::Node* node = musicModel->itemAt(sourceIndex);
        switch (node->type) {
            case MusicTreeModel::Node::Type::Album: {
                auto* data = musicModel->albumData(node);
                auto trackNames = musicModel->trackNamesForAlbum(node);
                m_albumSheet->setData(*data, trackNames);
                m_sheets->setCurrentWidget(m_albumSheet);
                break;
            }
            case MusicTreeModel::Node::Type::Audio:  {
                auto* data = musicModel->audioData(node);
                m_trackSheet->setData(*data);
                m_sheets->setCurrentWidget(m_trackSheet);
                break;
            }
        }
        return;
    }
}

void MainWindow::resetModelsAndUI() {
    m_movieModel.clearModel();
    m_seriesModel.clearModel();
    m_homeVideoModel.clearModel();
    m_musicVideoModel.clearModel();
    m_musicModel.clearModel();
    m_movies = {};
    m_series = {};
    m_homeVideos = {};
    m_musicVideos = {};
    m_music = {};
    m_stats->setText("");
    m_collectionBox->clear();
    m_collection = {};
    m_itemCount = 0;
    m_mainSplitter->setVisible(false);
    m_splashDialog.show();
}

void MainWindow::showMessage(msgStatus st, QString msg, int timeout) {
    if (!m_statusIcon || !m_statusText) return;
    m_statusText->setStyleSheet("color: palette(text); font-weight: normal;");
    QIcon icon;
    switch (st) {
    case information:
        icon = QIcon(":/status/assets/status_Information.svg");
        break;
    case warning:
        icon = QIcon(":/status/assets/status_Warning.svg");
        break;
    case error:
        icon = QIcon(":/status/assets/status_Error.svg");
        m_statusText->setStyleSheet("color: red; font-weight: bold;");
        break;
    case clear:
        m_statusIcon->clear();
        m_statusText->clear();
        m_statusText->setStyleSheet("");
        return;
    }
    m_statusIcon->setPixmap(icon.pixmap(16, 16));
    m_statusText->setText(msg);
    if (timeout > 0) m_statusTimer->start(timeout);
}
