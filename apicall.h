/////////////////////////////////////////////////////////////////////////////
// Name:        apicall.h
// Purpose:     Convenience layer for calls to Go library
// Author:      Jan Buchholz
// Created:     2026-05-02
// Changed:     2026-05-12
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "jbparser.hpp"
#include "jbgolib.hpp"
#include "globals.h"
#include <QString>

class APICall {

public:
    APICall() = delete;
    static void setAPI(GoLib* api);

    struct ApiResult {
        int code;
        std::string message;
    };

    static ApiResult userLoginToServer(bool secure, QString& host, QString& port, QString& name, QString& pass);
    static UserViewsResult userGetViews();
    static MoviesDataImp userGetMovieData(const std::string& collectionId);
    static SeriesDataImp userGetSeriesData(const std::string& collectionId);
    static HomeVideosDataImp userGetHomeVideoData(const std::string& collectionId);
    static MusicVideosDataImp userGetMusicVideoData(const std::string& collectionId);
    static MusicDataImp userGetMusicData(const std::string& collectionId);
    static ItemImageImp getPrimaryImageForItem(const std::string& itemId, const std::string& tag, bool exp=false);
    static void sendNetworkBroadcast();
    static bool getLoginResult();
    static QString getEmbyBaseUrl();
    static QString getBackendAPIVersion();

private:
    static EmbyLogonResult m_login;
    static GoLib* m_api;
};


