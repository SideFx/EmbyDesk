/////////////////////////////////////////////////////////////////////////////
// Name:        jbgolib.hpp
// Purpose:     C++ function import (load library)
// Author:      Jan Buchholz
// Created:     2026-04-20
// Last update: 2026-05-18
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <iostream>

#if defined(_WIN32)
    #include <windows.h>
    using LibHandle = HMODULE;
    #define LOAD_LIB(name) LoadLibraryA(name)
    #define GET_SYM(lib, name) GetProcAddress((HMODULE)lib, name)
    #define LIB_NAME "jbembyapi.dll"
#elif defined(__APPLE__)
    #include <dlfcn.h>
    using LibHandle = void*;
    #define LOAD_LIB(name) dlopen(name, RTLD_LAZY)
    #define GET_SYM(lib, name) dlsym(lib, name)
    #define LIB_NAME "libjbembyapi.dylib"
#else
    #include <dlfcn.h>
    using LibHandle = void*;
    #define LOAD_LIB(name) dlopen(name, RTLD_LAZY)
    #define GET_SYM(lib, name) dlsym(lib, name)
    #define LIB_NAME "libjbembyapi.so"
#endif

class GoLib {
public:
    using SendNetworkBroadcastFunc = void (*)();
    using UserLoginToEmbyServerFunc = char* (*)(bool, char*, char*, char*, char*);
    using UserGetViewsFunc = char* (*)(char*, char*, char*);
    using UserGetMoviesFunc = char* (*)(char*, char*, char*, char*);
    using UserGetSeriesFunc = char* (*)(char*, char*, char*, char*);
    using UserGetHomeVideosFunc = char* (*)(char*, char*, char*, char*);
    using UserGetMusicVideosFunc = char* (*)(char*, char*, char*, char*);
    using UserGetMusicFunc = char* (*)(char*, char*, char*, char*);
    using GetPrimaryImageForItemFunc = char* (*)(char*, char*, char*, char*, int, int, char*, bool);
    using FreeStringFunc = void (*)(char*);
    GoLib() = default;
    ~GoLib() = default; //Go shared libraries should not be unloaded
    bool load(const char* name = LIB_NAME) {
#ifdef __APPLE__
        std::string fullPath = std::string("@executable_path/../Frameworks/") + name;
        handle = LOAD_LIB(fullPath.c_str());
#else
        handle = LOAD_LIB(name);
#endif
        if (!handle) {
            std::cerr << "Failed to load library: " << name << "\n";
            return false;
        }
        sendNetworkBroadcast = reinterpret_cast<SendNetworkBroadcastFunc>(
            GET_SYM(handle, "SendNetworkBroadcast")
        );
        userLoginToEmbyServer = reinterpret_cast<UserLoginToEmbyServerFunc>(
            GET_SYM(handle, "UserLoginToEmbyServer")
        );
        userGetViews = reinterpret_cast<UserGetViewsFunc>(
            GET_SYM(handle, "UserGetViews")
        );
        userGetMovies = reinterpret_cast<UserGetMoviesFunc>(
            GET_SYM(handle, "UserGetMovies")
        );
        userGetSeries = reinterpret_cast<UserGetSeriesFunc>(
            GET_SYM(handle, "UserGetSeries")
        );
        userGetHomeVideos = reinterpret_cast<UserGetHomeVideosFunc>(
            GET_SYM(handle, "UserGetHomeVideos")
        );
        userGetMusicVideos = reinterpret_cast<UserGetMusicVideosFunc>(
            GET_SYM(handle, "UserGetMusicVideos")
        );
        userGetMusic = reinterpret_cast<UserGetMusicFunc>(
            GET_SYM(handle, "UserGetMusic")
        );
        getPrimaryImageForItem = reinterpret_cast<GetPrimaryImageForItemFunc>(
            GET_SYM(handle, "GetPrimaryImageForItem")
        );
        freeString = reinterpret_cast<FreeStringFunc>(
            GET_SYM(handle, "FreeString")
        );
        if (!sendNetworkBroadcast) {
            std::cerr << "Failed to load function <SendNetworkBroadcast>!\n";
            return false;
        }
        if (!userLoginToEmbyServer) {
            std::cerr << "Failed to load function <UserLoginToEmbyServer>!\n";
            return false;
        }
        if (!userGetViews) {
            std::cerr << "Failed to load function <UserGetViews>!\n";
            return false;
        }
        if (!userGetMovies) {
            std::cerr << "Failed to load function <UserGetMovies>!\n";
            return false;
        }
        if (!userGetSeries) {
            std::cerr << "Failed to load function <UserGetSeries>!\n";
            return false;
        }
        if (!userGetHomeVideos) {
            std::cerr << "Failed to load function <UserGetHomeVideos>!\n";
            return false;
        }
        if (!userGetMusicVideos) {
            std::cerr << "Failed to load function <UserGetMusicVideos>!\n";
            return false;
        }
        if (!userGetMusic) {
            std::cerr << "Failed to load function <UserGetMusic>!\n";
            return false;
        }
        if (!getPrimaryImageForItem) {
            std::cerr << "Failed to load function <GetPrimaryImageForItem>!\n";
            return false;
        }
        if (!freeString) {
            std::cerr << "Failed to load function <FreeString>!\n";
            return false;
        }
        return true;
    }
    void SendNetworkBroadcast() const {
        sendNetworkBroadcast();
    }
    [[nodiscard]] std::string UserLoginToEmbyServer(const bool secure, const std::string& host,
        const std::string& port, const std::string& username, const std::string& password) const {
        if (!userLoginToEmbyServer) return {};
        char* raw = userLoginToEmbyServer(
            secure,
            const_cast<char*>(host.c_str()),
            const_cast<char*>(port.c_str()),
            const_cast<char*>(username.c_str()),
            const_cast<char*>(password.c_str())
        );
        if (!raw) return {};
        std::string result(raw);
        freeString(raw);
        return result;
    }
    [[nodiscard]] std::string UserGetViews(const std::string& baseurl, const std::string& userid,
        const std::string& accesstoken) const {
        if (!userGetViews) return {};
        char* raw = userGetViews(
            const_cast<char*>(baseurl.c_str()),
            const_cast<char*>(userid.c_str()),
            const_cast<char*>(accesstoken.c_str())
        );
        if (!raw) return {};
        std::string result(raw);
        freeString(raw);
        return result;
    }
    [[nodiscard]] std::string UserGetMovies(const std::string& baseurl, const std::string& collectionid,
        const std::string& userid, const std::string& accesstoken) const {
        if (!userGetMovies) return {};
        char* raw = userGetMovies(
            const_cast<char*>(baseurl.c_str()),
            const_cast<char*>(collectionid.c_str()),
            const_cast<char*>(userid.c_str()),
            const_cast<char*>(accesstoken.c_str())
        );
        if (!raw) return {};
        std::string result(raw);
        freeString(raw);
        return result;
    }
    [[nodiscard]] std::string UserGetSeries(const std::string& baseurl, const std::string& collectionid,
        const std::string& userid, const std::string& accesstoken) const {
        if (!userGetSeries) return {};
        char* raw = userGetSeries(
            const_cast<char*>(baseurl.c_str()),
            const_cast<char*>(collectionid.c_str()),
            const_cast<char*>(userid.c_str()),
            const_cast<char*>(accesstoken.c_str())
        );
        if (!raw) return {};
        std::string result(raw);
        freeString(raw);
        return result;
    }
    [[nodiscard]] std::string UserGetHomeVideos(const std::string& baseurl, const std::string& collectionid,
        const std::string& userid, const std::string& accesstoken) const {
        if (!userGetHomeVideos) return {};
        char* raw = userGetHomeVideos(
            const_cast<char*>(baseurl.c_str()),
            const_cast<char*>(collectionid.c_str()),
            const_cast<char*>(userid.c_str()),
            const_cast<char*>(accesstoken.c_str())
        );
        if (!raw) return {};
        std::string result(raw);
        freeString(raw);
        return result;
    }
    [[nodiscard]] std::string UserGetMusicVideos(const std::string& baseurl, const std::string& collectionid,
        const std::string& userid, const std::string& accesstoken) const {
        if (!userGetMusicVideos) return {};
        char* raw = userGetMusicVideos(
            const_cast<char*>(baseurl.c_str()),
            const_cast<char*>(collectionid.c_str()),
            const_cast<char*>(userid.c_str()),
            const_cast<char*>(accesstoken.c_str())
        );
        if (!raw) return {};
        std::string result(raw);
        freeString(raw);
        return result;
    }
    [[nodiscard]] std::string UserGetMusic(const std::string& baseurl, const std::string& collectionid,
        const std::string& userid, const std::string& accesstoken) const {
        if (!userGetMusic) return {};
        char* raw = userGetMusic(
            const_cast<char*>(baseurl.c_str()),
            const_cast<char*>(collectionid.c_str()),
            const_cast<char*>(userid.c_str()),
            const_cast<char*>(accesstoken.c_str())
        );
        if (!raw) return {};
        std::string result(raw);
        freeString(raw);
        return result;
    }
    [[nodiscard]] std::string GetPrimaryImageForItem(const std::string& baseurl, const std::string& itemid,
        const std::string& format, const std::string& imagetag, int maxwidth, int maxheight,
        const std::string& accesstoken, bool exp) const {
        if (!getPrimaryImageForItem) return {};
        char* raw = getPrimaryImageForItem(
            const_cast<char*>(baseurl.c_str()),
            const_cast<char*>(itemid.c_str()),
            const_cast<char*>(format.c_str()),
            const_cast<char*>(imagetag.c_str()),
            maxwidth,
            maxheight,
            const_cast<char*>(accesstoken.c_str()),
            exp
        );
        if (!raw) return {};
        std::string result(raw);
        freeString(raw);
        return result;
    }
private:
    LibHandle handle = nullptr;
    SendNetworkBroadcastFunc sendNetworkBroadcast = nullptr;
    UserLoginToEmbyServerFunc userLoginToEmbyServer = nullptr;
    UserGetViewsFunc userGetViews = nullptr;
    UserGetMoviesFunc userGetMovies = nullptr;
    UserGetSeriesFunc userGetSeries = nullptr;
    UserGetHomeVideosFunc userGetHomeVideos = nullptr;
    UserGetMusicVideosFunc userGetMusicVideos = nullptr;
    UserGetMusicFunc userGetMusic = nullptr;
    GetPrimaryImageForItemFunc getPrimaryImageForItem = nullptr;
    FreeStringFunc freeString = nullptr;
};
