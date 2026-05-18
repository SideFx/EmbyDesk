/////////////////////////////////////////////////////////////////////////////
// Name:        mainwindow.h
// Purpose:     The main window
// Author:      Jan Buchholz
// Created:     2026-04-23
// Changed:     2026-05-17
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "jbparser.hpp"
#include <QMainWindow>
#include <QSplitter>
#include <QComboBox>
#include <QToolBar>
#include <QTreeView>
#include <QStackedWidget>
#include <QCloseEvent>
#include <QSortFilterProxyModel>
#include "metatypes.h"
#include "splashdialog.h"
#include "embydumpdialog.h"
#include "sqlreader.h"
#include "sheets.hpp"
#include "models/movietreemodel.h"
#include "models/seriestreemodel.h"
#include "models/homevideotreemodel.h"
#include "models/musicvideotreemodel.h"
#include "models/musictreemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow* ui;
    QToolBar* m_mainToolBar;
    QStatusBar* m_statusBar;
    QLabel* m_statusIcon;
    QLabel* m_statusText;
    QLabel* m_stats;
    QTimer* m_statusTimer;
    QSplitter* m_mainSplitter;
    QComboBox* m_collectionBox;
    QTreeView* m_tree;
    QStackedWidget* m_sheets;
    SplashDialog m_splashDialog;
    EmbyDumpDialog m_dumpDialog;
    SqlReader m_sqlReader;

    EmbyCollection m_collection;
    int m_itemCount = 0;
    bool m_offlineMode;
    QString m_lastFolderXlsx;
    QString m_lastFolderDb;
    QString m_lastUsedDb;

    MoviesDataImp m_movies;
    SeriesDataImp m_series;
    HomeVideosDataImp m_homeVideos;
    MusicVideosDataImp m_musicVideos;
    MusicDataImp m_music;
//  ---Sheets for StackedWidget---
    MovieDetailSheet* m_movieSheet;
    SeriesDetailSheet* m_seriesSheet;
    SeasonDetailSheet* m_seasonSheet;
    EpisodeDetailSheet* m_episodeSheet;
    HomeVideoDetailSheet* m_homeVideoSheet;
    MusicVideoDetailSheet* m_musicVideoSheet;
    AlbumDetailSheet* m_albumSheet;
    TrackDetailSheet* m_trackSheet;
//  ---Models and sort proxies---
    MovieTreeModel m_movieModel;
    QSortFilterProxyModel* m_movieProxy = nullptr;
    HomeVideoTreeModel m_homeVideoModel;
    QSortFilterProxyModel* m_homeVideoProxy = nullptr;
    MusicVideoTreeModel m_musicVideoModel;
    QSortFilterProxyModel* m_musicVideoProxy = nullptr;
    SeriesTreeModel m_seriesModel;
    MusicTreeModel m_musicModel;

    enum msgStatus {information, warning, error, clear};
    void createToolBar();
    void createMainControls();
    void createStatusBar();
    void setConnections();
    void showSplashScreen();
    void closeEvent(QCloseEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void centerSplashDialog(SplashDialog& dlg);
    void saveSettings();
    void loadSettings();
    void enableFunctions();
    void setProxyModels();
    void resetModelsAndUI();
    void showMessage(msgStatus st, QString msg, int timeout = 0);
    QIcon getIconForCollectionType(const std::string& type);
    void loadSheetForItem(const QModelIndex &sourceIndex);

private slots:
    void onActionSettings();
    void onActionLogin();
    void onActionRetrieve();
    void onActionExport();
    void onActionAbout();
    void onActionQuit();
    void onActionDump();
    void onActionOffline();
    // ---tree line selection---
    void onCurrentChanged(const QModelIndex&,  const QModelIndex&);
    // ---collection dropdown box---
    void onCollectionChanged(int);
    // ---get db-name/path from dump dialog---
    void onReceivePathInfo(const QString&, const QString&);
};

