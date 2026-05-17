/////////////////////////////////////////////////////////////////////////////
// Name:        apicall.cpp
// Purpose:     Convenience layer for calls to Go library
// Author:      Jan Buchholz
// Created:     2026-05-02
// Changed:     2026-05-12
/////////////////////////////////////////////////////////////////////////////

#include "apicall.h"
#include "conversions.hpp"

EmbyLogonResult APICall::m_login{.code = -1};
GoLib* APICall::m_api = nullptr;

void APICall::setAPI(GoLib* api) {
    m_api = api;
}

APICall::ApiResult APICall::userLoginToServer(bool secure,
                                              QString& host,
                                              QString& port,
                                              QString& name,
                                              QString& pass) {
    std::string raw = m_api->UserLoginToEmbyServer(secure,
                                                   toStandardString(host),
                                                   toStandardString(port),
                                                   toStandardString(name),
                                                   toStandardString(pass));
    m_login = parseLogonResult(raw);
    return {.code = m_login.code, .message = m_login.message};
}

UserViewsResult APICall::userGetViews() {
    std::string raw = m_api->UserGetViews(m_login.baseUrl,
                                          m_login.userId,
                                          m_login.accessToken);
    UserViewsResult views = parseUserViews(raw);
    // ---Sort collections by name---
    std::stable_sort(views.views.begin(),
                     views.views.end(),
                     [](const UserView &a, const UserView &b) {
        return a.name < b.name;
    });
    return views;
}

MoviesDataImp APICall::userGetMovieData(const std::string& collectionId) {
    std::string raw = m_api->UserGetMovies(m_login.baseUrl,
                                           collectionId,
                                           m_login.userId,
                                           m_login.accessToken);
    return parseMovies(raw);
}

SeriesDataImp APICall::userGetSeriesData(const std::string& collectionId) {
    std::string raw = m_api->UserGetSeries(m_login.baseUrl,
                                           collectionId,
                                           m_login.userId,
                                           m_login.accessToken);
    return parseSeries(raw);
}

HomeVideosDataImp APICall::userGetHomeVideoData(const std::string& collectionId) {
    std::string raw = m_api->UserGetHomeVideos(m_login.baseUrl,
                                               collectionId,
                                               m_login.userId,
                                               m_login.accessToken);
    return parseHomeVideos(raw);
}

MusicVideosDataImp APICall::userGetMusicVideoData(const std::string& collectionId) {
    std::string raw = m_api->UserGetMusicVideos(m_login.baseUrl,
                                                collectionId,
                                                m_login.userId,
                                                m_login.accessToken);
    return parseMusicVideos(raw);
}

MusicDataImp APICall::userGetMusicData(const std::string& collectionId) {
    std::string raw = m_api->UserGetMusic(m_login.baseUrl,
                                          collectionId,
                                          m_login.userId,
                                          m_login.accessToken);
    return parseMusic(raw);
}

ItemImageImp APICall::getPrimaryImageForItem(const std::string& itemId,
                                             const std::string& tag,
                                             bool exp) {
    std::string raw = m_api->GetPrimaryImageForItem(m_login.baseUrl,
                                                    itemId,
                                                    "png",
                                                    tag,
                                                    DEF_IMAGE_WIDTH,
                                                    DEF_IMAGE_HEIGHT,
                                                    m_login.accessToken,
                                                    exp);
    return parseItemImage(raw);
}

void APICall::sendNetworkBroadcast() {
    m_api->SendNetworkBroadcast();
}

QString APICall::getEmbyBaseUrl() {
    return toQString(m_login.baseUrl);
}

QString APICall::getBackendAPIVersion() {
    return toQString(m_login.apiVersion);
}

bool APICall::getLoginResult() {
    return m_login.code == 0;
}
