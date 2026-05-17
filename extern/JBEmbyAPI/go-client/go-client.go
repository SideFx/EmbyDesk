/////////////////////////////////////////////////////////////////////////////
// Name:        go-client.go
// Purpose:     Demo console application for JBEmbyAPI
// Author:      Jan Buchholz
// Created:     2026-04-13
// Last update: 2026-05-01
/////////////////////////////////////////////////////////////////////////////

package main

import (
	"JBEmbyAPI/API"
	"flag"
	"fmt"
	"os"
)

func main() {
	useHttps := flag.Bool("https", false, "Use HTTPS instead of HTTP")
	host := flag.String("host", "localhost", "Hostname or IP address")
	port := flag.String("port", "8096", "Port of the Emby server")
	user := flag.String("user", "", "Username")
	pass := flag.String("pass", "", "Password")
	flag.Parse()
	// Check if all required parameters are provided
	if *host == "" || *port == "" || *user == "" || *pass == "" {
		fmt.Println("Usage:")
		fmt.Println("  go-client -https=true|false -host=<host> -port=<port> -user=<username> -pass=<password>")
		os.Exit(1)
	}
	protocol := "http"
	if *useHttps {
		protocol = "https"
	}
	API.SendNetworkBroadcast() // should normally be called during UI init
	baseUrl := fmt.Sprintf("%s://%s:%s", protocol, *host, *port)
	fmt.Println("Connecting to:", baseUrl)
	login := API.UserLoginToServer(
		*useHttps,
		*host,
		*port,
		*user,
		*pass,
	)
	fmt.Println("Message:", login.Result.Message)
	if login.Result.Code == 0 {
		fmt.Println("Base URL: ", login.Session.BaseUrl)
		fmt.Println("Token: ", login.Session.AccessToken)
		fmt.Println("User ID: ", login.Session.UserId)
	} else {
		fmt.Println("Login failed.")
		os.Exit(2)
	}
	// Retrieve all views
	views := API.UserGetViews(login.Session.BaseUrl, login.Session.UserId, login.Session.AccessToken)
	if views.Result.Code != API.NoErrorConst {
		fmt.Println("Error:", views.Result.Message)
		os.Exit(3)
	}
	if len(views.UserViews) == 0 {
		fmt.Println("No views for user found.")
		os.Exit(0)
	}
	fmt.Println("\nViews (Name, Type, ID):")
	for _, view := range views.UserViews {
		fmt.Println(view.Name, view.CollectionType, view.Id)
	}
	fmt.Println("\nFetching items:")
	for _, view := range views.UserViews {
		fmt.Println("\n\nFetching items for view:", view.Name)
		switch view.CollectionType {
		case API.CollectionMovies:
			movies := API.UserGetMovies(login.Session.BaseUrl,
				view.Id,
				login.Session.UserId,
				login.Session.AccessToken)
			if movies.Result.Code == API.NoErrorConst {
				for _, m := range movies.Movies.TMovieData {
					fmt.Println("Name: ", m.Name,
						"Year: ", m.ProductionYear,
						"Runtime: ", m.Runtime,
						"Width: ", m.Width,
						"Height: ", m.Height,
						"Bitrate: ", m.Bitrate,
						"Filesize: ", m.FileSize,
						"AdddedAt: ", m.AddedAt)
				}
				for _, f := range movies.Movies.TFolderData {
					fmt.Println("FOLDER:", f.Name, f.FolderId)
				}
			} else {
				fmt.Println(movies.Result.Message)
			}
			break
		case API.CollectionSeries:
			series := API.UserGetSeries(login.Session.BaseUrl,
				view.Id,
				login.Session.UserId,
				login.Session.AccessToken)
			if series.Result.Code == API.NoErrorConst {
				for _, s := range series.Series.TSeriesData {
					fmt.Println(s.Name, s.AddedAt)
				}
			} else {
				fmt.Println(series.Result.Message)
			}
			break
		case API.CollectionHomeVideos:
			videos := API.UserGetHomeVideos(login.Session.BaseUrl,
				view.Id,
				login.Session.UserId,
				login.Session.AccessToken)
			if videos.Result.Code == API.NoErrorConst {
				for _, video := range videos.HomeVideos.THomeVideoData {
					fmt.Println(video.Name, video.AddedAt, video.FileName)
				}
				for _, f := range videos.HomeVideos.TFolderData {
					fmt.Println("FOLDER:", f.Name, f.FolderId)
				}
			} else {
				fmt.Println(videos.Result.Message)
			}
			break
		case API.CollectionMusicVideos:
			videos := API.UserGetMusicVideos(login.Session.BaseUrl,
				view.Id,
				login.Session.UserId,
				login.Session.AccessToken)
			if videos.Result.Code == API.NoErrorConst {
				for _, v := range videos.MusicVideos.TMusicVideoData {
					fmt.Println(v.Name, v.AddedAt, v.FileName)
				}
				for _, f := range videos.MusicVideos.TFolderData {
					fmt.Println("FOLDER:", f.Name, f.FolderId)
				}
			} else {
				fmt.Println(videos.Result.Message)
			}
			break
		case API.CollectionMusic:
			music := API.UserGetMusic(login.Session.BaseUrl,
				view.Id,
				login.Session.UserId,
				login.Session.AccessToken)
			if music.Result.Code == API.NoErrorConst {
				for _, m := range music.Music.TAudioData {
					fmt.Println(m.Name, m.Bitrate)
				}
			} else {
				fmt.Println(music.Result.Message)
			}
		}
	}
}
