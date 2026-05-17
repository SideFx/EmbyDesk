/////////////////////////////////////////////////////////////////////////////
// Name:        main.cpp
// Purpose:     Demo console application for JBEmbyAPI
// Author:      Jan Buchholz
// Created:     2026-04-21
// Last update: 2026-05-02
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <unordered_map>
#include "jbparser.hpp"
#include "jbgolib.hpp"

// Command line parser
std::unordered_map<std::string, std::string> parseArgs(int argc, char* argv[]) {
    std::unordered_map<std::string, std::string> args;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a.rfind('-', 0) == 0) { // parameters start with '-'
            auto eq = a.find('=');
            if (eq != std::string::npos) {
                std::string key = a.substr(1, eq - 1);
                std::string val = a.substr(eq + 1);
                args[key] = val;
            }
        }
    }
    return args;
}

int main(int argc, char* argv[]) {
    GoLib lib;
    if (!lib.load()) {
        return -1;
    }
#if defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    auto args = parseArgs(argc, argv);
    // Check if all required parameters are provided
    if (!args.contains("https") ||
        !args.contains("host") ||
        !args.contains("port") ||
        !args.contains("user") ||
        !args.contains("pass")) {
        std::cout << "Usage:\n";
        std::cout << "  app -https=true|false -host=<host> -port=<port> -user=<username> -pass=<password>\n";
        return 1;
    }
    lib.SendNetworkBroadcast(); // should normally be called during UI init
    bool useHttps = (args["https"] == "true");
    std::string host = args["host"];
    std::string port = args["port"];
    std::string user = args["user"];
    std::string pass = args["pass"];
    std::cout << "Connecting to " << host << ":" << port
              << " (HTTPS=" << (useHttps ? "true" : "false") << ")\n";
    std::string raw;
    raw = lib.UserLoginToEmbyServer(useHttps, host, port, user, pass);
    EmbyLogonResult login = parseLogonResult(raw);
    if (login.code == 0) {
        std::cout << "Message: " << login.message << "\n";
        std::cout << "AccessToken: " << login.accessToken << "\n";
        std::cout << "UserId: " << login.userId << "\n";
    } else {
        std::cout << "Failed to login with message: " << login.message << "\n";
        exit(1);
    }
    // Retrieve all views
    raw = lib.UserGetViews(login.baseUrl, login.userId, login.accessToken);
    UserViewsResult views = parseUserViews(raw);
    if (views.code != 0) {
        std::cout << "Error: " << views.message << "\n";
        exit(2);
    }
    if (views.views.empty()) {
        std::cout << "No views for user found.\n";
        exit(3);
    }
    std::cout << "\n\nViews (Name, Type, ID):\n";
    for (auto& view : views.views) {
        std::cout << view.name << " " << view.collectionType << " " << view.id << "\n";
    }
    std::cout << "\n\nFetching items:\n";
    for (auto& item : views.views) {
        std::cout << "\n\nFetching items for view: " << item.name << "\n";
        if (item.collectionType == CollectionMovies) {
            raw = lib.UserGetMovies(login.baseUrl, item.id, login.userId, login.accessToken);
            MoviesDataImp moviesImp = parseMovies(raw);
            if (moviesImp.code == 0) {
                // Movies: moviesImp.movies.tMovieData
                // Folders: moviesImp.movies.tFolderData
                for (auto& m : moviesImp.movies.tMovieData) {
                    std::cout << m.name << " " << m.addedAt << " " << m.type << "\n";
                }
                for (auto& f : moviesImp.movies.tFolderData) {
                    std::cout << "FOLDER: " << f.name << " " << f.folderId << "\n";
                }
            } else {
                std::cout << "Error: " << moviesImp.message << "\n";
            }
            continue;
        }
        if (item.collectionType == CollectionSeries) {
            raw = lib.UserGetSeries(login.baseUrl, item.id, login.userId, login.accessToken);
            SeriesDataImp seriesImp = parseSeries(raw);
            if (seriesImp.code == 0) {
                // Series: seriesImp.series.tSeriesData
                // Seasons: seriesImp.series.tSeasonData
                // Episodes: seriesImp.series.tEpisodeData
                for (auto& s : seriesImp.series.tSeriesData) {
                    std::cout << s.name << " " << s.addedAt << " " << s.type << "\n";
                }
            } else {
                std::cout << "Error: " << seriesImp.message << "\n";
            }
            continue;
        }
        if (item.collectionType == CollectionHomeVideos) {
            raw = lib.UserGetHomeVideos(login.baseUrl, item.id, login.userId, login.accessToken);
            HomeVideosDataImp homeVideosImp = parseHomeVideos(raw);
            if (homeVideosImp.code == 0) {
                // HomeVideos: homeVideosImp.homeVideos.tHomeVideoData
                // Folders: homeVideosImp.homeVideos.tFolderData
                for (auto& h : homeVideosImp.homeVideos.tHomeVideoData) {
                    std::cout << h.name << " " << h.addedAt << " " << h.type << "\n";
                }
                for (auto& f : homeVideosImp.homeVideos.tFolderData) {
                    std::cout << "FOLDER: " << f.name << " " << f.folderId << "\n";
                }
            } else {
                std::cout << "Error: " << homeVideosImp.message << "\n";
            }
            continue;
        }
        if (item.collectionType == CollectionMusicVideos) {
            raw = lib.UserGetMusicVideos(login.baseUrl, item.id, login.userId, login.accessToken);
            MusicVideosDataImp musicVideosImp = parseMusicVideos(raw);
            if (musicVideosImp.code == 0) {
                // MusicVideos: musicVideosImp.musicVideos.tMusicVideoData
                // Folders: musicVideosImp.musicVideos.tFolderData
                for (auto& m : musicVideosImp.musicVideos.tMusicVideoData) {
                    std::cout << m.name << " " << m.addedAt << " " << m.type << "\n";
                }
                for (auto& f : musicVideosImp.musicVideos.tFolderData) {
                    std::cout << "FOLDER: " << f.name << " " << f.folderId << "\n";
                }
            } else {
                std::cout << "Error: " << musicVideosImp.message << "\n";
            }
            continue;
        }
        if (item.collectionType == CollectionMusic) {
            raw = lib.UserGetMusic(login.baseUrl, item.id, login.userId, login.accessToken);
            MusicDataImp musicImp = parseMusic(raw);
            if (musicImp.code == 0) {
                // Albums: musicImp.music.tAlbumData
                // Titles: musicImp.music.tAudioData
                for (auto& m : musicImp.music.tAlbumData) {
                    std::cout << m.name << " " << m.addedAt << " " << m.type << "\n";
                }
            } else {
                std::cout << "Error: " << musicImp.message << "\n";
            }
        }
    }
}
