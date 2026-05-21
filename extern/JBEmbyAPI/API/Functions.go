/////////////////////////////////////////////////////////////////////////////
// Name:        Functions.go
// Purpose:     Exported functions
// Author:      Jan Buchholz
// Created:     2026-04-15
// Last update: 2026-05-21
/////////////////////////////////////////////////////////////////////////////

package API

import (
	"fmt"
	"io"
	"net/http"
	"strconv"
	"strings"
	"time"
)

func UserLoginToServer(secure bool, hostname string, port string, username string, password string) EmbyLogonResultExp {
	result := EmbyLogonResultExp{}
	url := createBasicURL(secure, hostname, port)
	result.Session.BaseUrl = url.BaseUrl
	result.Result = url.Result
	result.APIVersion = ApiVersion
	if url.Result.Code != NoErrorConst {
		return result
	}
	id := findUserIdByName(url.BaseUrl, username)
	result.Session.UserId = id.Id
	result.Result = id.Result
	if id.Result.Code != NoErrorConst {
		return result
	}
	tk := authenticateUserByCredentials(url.BaseUrl, id.Id, username, password)
	result.Session.AccessToken = tk.Token
	result.Result = tk.Result
	return result
}

func UserGetViews(baseurl string, userid string, accesstoken string) UserViewsExp {
	var views UserViewsExp
	p := rESTParams[QueryResultBaseItemDto]{}
	p.url = baseurl + getUserViews
	p.url = strings.Replace(p.url, "&1", userid, 1)
	p.url += "?" + paraApiKey + accesstoken
	p.genericHttpGet()
	if p.error.Code != NoErrorConst {
		views.Result = p.error
		return views
	}
	for _, item := range p.data.Items {
		for _, collectionType := range SupportedCollectionTypes {
			if item.CollectionType == collectionType {
				var v = UserView{
					Name:           item.Name,
					CollectionType: item.CollectionType,
					Id:             item.Id,
				}
				views.UserViews = append(views.UserViews, v)
			}
		}
	}
	views.Result = NoError
	return views
}

func UserGetMovies(baseurl string, collectionid string, userid string, accesstoken string) MoviesDataExp {
	MovieTable.Data = MovieData{}
	err := checkCollectionType(baseurl, userid, accesstoken, collectionid, MovieTable.CollectionType)
	if err.Code != NoErrorConst {
		return MoviesDataExp{MovieData{}, err}
	}
	items := userGetItems(baseurl, collectionid, MovieTable.CollectionType, userid, accesstoken)
	if items.Result.Code != NoErrorConst {
		return MoviesDataExp{MovieData{}, items.Result}
	}
	for _, item := range items.Items {
		switch item.Type {
		case MovieType:
			var movie MovieDataInc
			movie.Name = item.Name
			movie.MovieId = item.Id
			movie.OriginalTitle = item.OriginalTitle
			movie.ProductionYear = item.ProductionYear
			movie.Studios = evalNameLongIdPairs(item.Studios)
			movie.Actors = evalPersons(item.People, ActorPersonType, GuestStarPersonType)
			movie.Directors = evalPersons(item.People, DirectorPersonType)
			movie.Genres = evalNameLongIdPairs(item.GenreItems)
			movie.Container = item.Container
			movie.AudioCodec, movie.VideoCodec = evalCodecs(item.MediaSources)
			movie.Width = item.Width
			movie.Height = item.Height
			movie.Bitrate = item.Bitrate
			movie.Runtime = item.RunTimeTicks
			movie.AddedAt = item.DateCreated.Unix()
			movie.PrimaryImageId = item.PrimaryImageItemId
			if movie.PrimaryImageId == "" {
				movie.PrimaryImageId = item.Id
			}
			movie.PrimaryImageTag = item.PrimaryImageTag
			if movie.PrimaryImageTag == "" {
				movie.PrimaryImageTag = item.ImageTags[PrimaryImage]
			}
			movie.FolderId = item.ParentId
			movie.FileSize = item.Size
			movie.FileName = item.FileName
			movie.Overview = item.Overview
			movie.ImDBId = item.ProviderIds[ImDb]
			movie.Type = item.Type
			MovieTable.Data.TMovieData = append(MovieTable.Data.TMovieData, movie)
		case FolderType:
			folder := FolderDataInc{}
			folder.Name = item.Name
			folder.FolderId = item.Id
			MovieTable.Data.TFolderData = append(MovieTable.Data.TFolderData, folder)
			break
		}
	}
	sortFoldersById(MovieTable.Data.TFolderData)
	return MoviesDataExp{MovieTable.Data, NoError}
}

func UserGetSeries(baseurl string, collectionid string, userid string, accesstoken string) SeriesDataExp {
	SeriesTable.Data = SeriesData{}
	err := checkCollectionType(baseurl, userid, accesstoken, collectionid, SeriesTable.CollectionType)
	if err.Code != NoErrorConst {
		return SeriesDataExp{SeriesData{}, err}
	}
	items := userGetItems(baseurl, collectionid, SeriesTable.CollectionType, userid, accesstoken)
	if items.Result.Code != NoErrorConst {
		return SeriesDataExp{SeriesData{}, items.Result}
	}
	for _, item := range items.Items {
		switch item.Type {
		case SeriesType:
			series := SeriesDataInc{}
			series.Name = item.Name
			series.OriginalTitle = item.OriginalTitle
			series.ProductionYear = item.ProductionYear
			series.Actors = evalPersons(item.People, ActorPersonType, GuestStarPersonType)
			series.Directors = evalPersons(item.People, DirectorPersonType)
			series.Genres = evalNameLongIdPairs(item.GenreItems)
			series.Studios = evalNameLongIdPairs(item.Studios)
			series.Overview = item.Overview
			series.AddedAt = item.DateCreated.Unix()
			series.PrimaryImageId = item.PrimaryImageItemId
			if series.PrimaryImageId == "" {
				series.PrimaryImageId = item.Id
			}
			series.PrimaryImageTag = item.PrimaryImageTag
			if series.PrimaryImageTag == "" {
				series.PrimaryImageTag = item.ImageTags[PrimaryImage]
			}
			series.ImDBId = item.ProviderIds[ImDb]
			series.SeriesId = item.Id
			series.Type = item.Type
			SeriesTable.Data.TSeriesData = append(SeriesTable.Data.TSeriesData, series)
			break
		case SeasonType:
			season := SeasonDataInc{}
			season.Name = item.Name
			season.SeriesId = item.SeriesId
			season.SeasonId = item.Id
			season.ProductionYear = item.ProductionYear
			season.AddedAt = item.DateCreated.Unix()
			season.PrimaryImageId = item.PrimaryImageItemId
			if season.PrimaryImageId == "" {
				season.PrimaryImageId = item.Id
			}
			season.PrimaryImageTag = item.PrimaryImageTag
			if season.PrimaryImageTag == "" {
				season.PrimaryImageTag = item.ImageTags[PrimaryImage]
			}
			season.SortIndex = item.IndexNumber
			season.Type = item.Type
			SeriesTable.Data.TSeasonData = append(SeriesTable.Data.TSeasonData, season)
			break
		case EpisodeType:
			episode := EpisodeDataInc{}
			episode.Name = item.Name
			episode.OriginalTitle = item.OriginalTitle
			episode.EpisodeId = item.Id
			episode.ProductionYear = item.ProductionYear
			episode.Actors = evalPersons(item.People, ActorPersonType, GuestStarPersonType)
			episode.Directors = evalPersons(item.People, DirectorPersonType)
			episode.Runtime = item.RunTimeTicks
			episode.Container = item.Container
			episode.AudioCodec, episode.VideoCodec = evalCodecs(item.MediaSources)
			episode.Width = item.Width
			episode.Height = item.Height
			episode.Bitrate = item.Bitrate
			episode.SortIndex = item.IndexNumber
			episode.FileSize = item.Size
			episode.FileName = item.FileName
			episode.Overview = item.Overview
			episode.AddedAt = item.DateCreated.Unix()
			episode.PrimaryImageId = item.PrimaryImageItemId
			if episode.PrimaryImageId == "" {
				episode.PrimaryImageId = item.Id
			}
			episode.PrimaryImageTag = item.PrimaryImageTag
			if episode.PrimaryImageTag == "" {
				episode.PrimaryImageTag = item.ImageTags[PrimaryImage]
			}
			episode.ImDBId = item.ProviderIds[ImDb]
			episode.SeriesId = item.SeriesId
			episode.SeasonId = item.SeasonId
			episode.Type = item.Type
			SeriesTable.Data.TEpisodeData = append(SeriesTable.Data.TEpisodeData, episode)
			break
		}
	}
	return SeriesDataExp{SeriesTable.Data, NoError}
}

func UserGetHomeVideos(baseurl string, collectionid string, userid string, accesstoken string) HomeVideosDataExp {
	HomeVideoTable.Data = HomeVideoData{}
	err := checkCollectionType(baseurl, userid, accesstoken, collectionid, HomeVideoTable.CollectionType)
	if err.Code != NoErrorConst {
		return HomeVideosDataExp{HomeVideoData{}, err}
	}
	items := userGetItems(baseurl, collectionid, HomeVideoTable.CollectionType, userid, accesstoken)
	if items.Result.Code != NoErrorConst {
		return HomeVideosDataExp{HomeVideoData{}, items.Result}
	}
	for _, item := range items.Items {
		switch item.Type {
		case VideoType:
			video := HomeVideoDataInc{}
			video.Name = item.Name
			video.ProductionYear = item.ProductionYear
			video.People = evalPersons(item.People, AnyPersonType)
			video.Genres = evalNameLongIdPairs(item.GenreItems)
			video.Overview = item.Overview
			video.Container = item.Container
			video.Width = item.Width
			video.Height = item.Height
			video.AudioCodec, video.VideoCodec = evalCodecs(item.MediaSources)
			video.Runtime = item.RunTimeTicks
			video.Bitrate = item.Bitrate
			video.FileSize = item.Size
			video.FileName = item.FileName
			video.PrimaryImageId = item.PrimaryImageItemId
			if video.PrimaryImageId == "" {
				video.PrimaryImageId = item.Id
			}
			video.PrimaryImageTag = item.PrimaryImageTag
			if video.PrimaryImageTag == "" {
				video.PrimaryImageTag = item.ImageTags[PrimaryImage]
			}
			video.AddedAt = item.DateCreated.Unix()
			video.VideoId = item.Id
			video.FolderId = item.ParentId
			video.Type = item.Type
			HomeVideoTable.Data.THomeVideoData = append(HomeVideoTable.Data.THomeVideoData, video)
			break
		case FolderType:
			folder := FolderDataInc{}
			folder.Name = item.Name
			folder.FolderId = item.Id
			HomeVideoTable.Data.TFolderData = append(HomeVideoTable.Data.TFolderData, folder)
			break
		}
	}
	sortFoldersById(HomeVideoTable.Data.TFolderData)
	return HomeVideosDataExp{HomeVideoTable.Data, NoError}
}

func UserGetMusicVideos(baseurl string, collectionid string, userid string, accesstoken string) MusicVideosDataExp {
	MusicVideoTable.Data = MusicVideoData{}
	err := checkCollectionType(baseurl, userid, accesstoken, collectionid, MusicVideoTable.CollectionType)
	if err.Code != NoErrorConst {
		return MusicVideosDataExp{MusicVideoData{}, err}
	}
	items := userGetItems(baseurl, collectionid, MusicVideoTable.CollectionType, userid, accesstoken)
	if items.Result.Code != NoErrorConst {
		return MusicVideosDataExp{MusicVideoData{}, items.Result}
	}
	for _, item := range items.Items {
		switch item.Type {
		case MusicVideoType:
			video := MusicVideoDataInc{}
			video.Name = item.Name
			video.ProductionYear = item.ProductionYear
			video.Genres = evalNameLongIdPairs(item.GenreItems)
			video.Artists = evalNameIdPairs(item.ArtistItems)
			video.Overview = item.Overview
			video.Container = item.Container
			video.Width = item.Width
			video.Height = item.Height
			video.AudioCodec, video.VideoCodec = evalCodecs(item.MediaSources)
			video.Runtime = item.RunTimeTicks
			video.Bitrate = item.Bitrate
			video.AddedAt = item.DateCreated.Unix()
			video.FileSize = item.Size
			video.FileName = item.FileName
			video.PrimaryImageId = item.PrimaryImageItemId
			if video.PrimaryImageId == "" {
				video.PrimaryImageId = item.Id
			}
			video.PrimaryImageTag = item.PrimaryImageTag
			if video.PrimaryImageTag == "" {
				video.PrimaryImageTag = item.ImageTags[PrimaryImage]
			}
			video.VideoId = item.Id
			video.FolderId = item.ParentId
			video.Type = item.Type
			MusicVideoTable.Data.TMusicVideoData = append(MusicVideoTable.Data.TMusicVideoData, video)
			break
		case FolderType:
			folder := FolderDataInc{}
			folder.Name = item.Name
			folder.FolderId = item.Id
			MusicVideoTable.Data.TFolderData = append(MusicVideoTable.Data.TFolderData, folder)
			break
		}
	}
	sortFoldersById(MusicVideoTable.Data.TFolderData)
	return MusicVideosDataExp{MusicVideoTable.Data, NoError}
}

func UserGetMusic(baseurl string, collectionid string, userid string, accesstoken string) MusicDataExp {
	MusicTable.Data = MusicData{}
	err := checkCollectionType(baseurl, userid, accesstoken, collectionid, MusicTable.CollectionType)
	if err.Code != NoErrorConst {
		return MusicDataExp{MusicData{}, err}
	}
	items := userGetItems(baseurl, collectionid, MusicTable.CollectionType, userid, accesstoken)
	if items.Result.Code != NoErrorConst {
		return MusicDataExp{MusicData{}, items.Result}
	}
	for _, item := range items.Items {
		switch item.Type {
		case AudioType:
			audio := AudioDataInc{}
			audio.Name = item.Name
			audio.ProductionYear = item.ProductionYear
			audio.Artists = evalNameIdPairs(item.ArtistItems)
			audio.Genres = evalNameLongIdPairs(item.GenreItems)
			audio.Container = item.Container
			audio.Album = item.Album
			audio.AlbumArtist = item.AlbumArtist
			if audio.AlbumArtist == "" {
				audio.AlbumArtist, _ = evalAlbumArtists(item.AlbumArtists)
			}
			audio.FileSize = item.Size
			audio.FileName = item.FileName
			audio.Bitrate = item.Bitrate
			audio.AudioCodec, _ = evalCodecs(item.MediaSources)
			audio.TrackNumber = item.IndexNumber
			audio.DiscNumber = item.ParentIndexNumber
			audio.Runtime = item.RunTimeTicks
			audio.MediaType = item.MediaType
			audio.AddedAt = item.DateCreated.Unix()
			audio.PrimaryImageId = item.PrimaryImageItemId
			if audio.PrimaryImageId == "" {
				audio.PrimaryImageId = item.Id
			}
			audio.PrimaryImageTag = item.PrimaryImageTag
			if audio.PrimaryImageTag == "" {
				audio.PrimaryImageTag = item.ImageTags[PrimaryImage]
			}
			audio.AudioId = item.Id
			audio.AlbumId = item.AlbumId
			audio.Type = item.Type
			MusicTable.Data.TAudioData = append(MusicTable.Data.TAudioData, audio)
			break
		case MusicAlbumType:
			album := AlbumDataInc{}
			album.Name = item.Name
			album.ProductionYear = item.ProductionYear
			album.Artists = evalNameIdPairs(item.ArtistItems)
			album.Genres = evalNameLongIdPairs(item.GenreItems)
			album.AlbumId = item.Id
			album.AlbumArtist = item.AlbumArtist
			if album.AlbumArtist == "" {
				album.AlbumArtist, _ = evalAlbumArtists(item.AlbumArtists)
			}
			album.Genres = evalNameLongIdPairs(item.GenreItems)
			album.Runtime = item.RunTimeTicks
			album.AddedAt = item.DateCreated.Unix()
			album.PrimaryImageId = item.PrimaryImageItemId
			if album.PrimaryImageId == "" {
				album.PrimaryImageId = item.Id
			}
			album.PrimaryImageTag = item.PrimaryImageTag
			if album.PrimaryImageTag == "" {
				album.PrimaryImageTag = item.ImageTags[PrimaryImage]
			}
			album.MusicBrainzId = item.ProviderIds[MusicBrainzAlbum]
			album.Type = item.Type
			MusicTable.Data.TAlbumData = append(MusicTable.Data.TAlbumData, album)
			break
		}
	}
	return MusicDataExp{MusicTable.Data, NoError}
}

func GetPrimaryImageForItem(baseurl string, itemid string, format string, imagetag string,
	maxwidth int, maxheight int, accesstoken string, export bool) ItemImageExp {
	img := ItemImageExp{}
	url := baseurl + getPrimaryImage
	url = strings.Replace(url, "&1", itemid, 1)
	url += "?" + paraApiKey + accesstoken
	if format == ImageFormatBmp || format == ImageFormatGif || format == ImageFormatJpp || format == ImageFormatPng {
		url += "&" + paraFormat + string(format)
	}
	if maxwidth > 0 {
		url += "&" + paraMaxWidth + strconv.Itoa(maxwidth)
	}
	if maxheight > 0 {
		url += "&" + paraMaxHeight + strconv.Itoa(maxheight)
	}
	if export == true {
		url += "&" + paraImageQuality
		url += "&" + paraNoEnhancers
	}
	if imagetag != "" {
		url += "&" + paraImageTag + imagetag
	}
	response, err := http.Get(url)
	defer func() { _ = response.Body.Close() }()
	if err != nil {
		return ItemImageExp{itemid, nil, HttpGetFailed}
	}
	if response.StatusCode != http.StatusOK {
		e := HttpStatusError
		e.Message = strings.Replace(e.Message, "&1", strconv.Itoa(response.StatusCode), -1)
		return ItemImageExp{itemid, nil, e}
	}
	img.ImageData, err = io.ReadAll(response.Body)
	if err != nil {
		return ItemImageExp{itemid, nil, IoError}
	}
	img.Result = NoError
	return img
}

func SendNetworkBroadcast() {
	go func() {
		for i := 0; i < 3; i++ {
			sendNetworkBroadcast()
			time.Sleep(1 * time.Second)
		}
	}()
}

// ---Optional Evaluation/Conversion Functions---

func EvalFileSize(filesize int64) string {
	const (
		KB = 1024
		MB = 1024 * KB
		GB = 1024 * MB
	)
	switch {
	case filesize < KB:
		return fmt.Sprintf("%d B", filesize)
	case filesize < MB:
		return fmt.Sprintf("%.2f KB", float64(filesize)/KB)
	case filesize < GB:
		return fmt.Sprintf("%.2f MB", float64(filesize)/MB)
	default:
		return fmt.Sprintf("%.2f GB", float64(filesize)/GB)
	}
}

func EvalBitrate(bitrate int32) string {
	const (
		K = 1000
		M = 1000 * K
		G = 1000 * M
	)
	b := float64(bitrate)
	switch {
	case b < K:
		return fmt.Sprintf("%d bps", bitrate)
	case b < M:
		return fmt.Sprintf("%.0f kbps", b/K)
	case b < G:
		return fmt.Sprintf("%.2f Mbps", b/M)
	default:
		return fmt.Sprintf("%.2f Gbps", b/G)
	}
}

func EvalTime(ts int64) string {
	return time.Unix(ts, 0).UTC().Format("2006-01-02") // ISO 8601 date format
}

func EvalRuntime(ticks int64) string {
	if ticks <= 0 {
		return ""
	}
	seconds := ticks / 10_000_000
	hours := seconds / 3600
	minutes := (seconds % 3600) / 60
	var b strings.Builder
	if hours > 0 {
		b.WriteString(strconv.Itoa(int(hours)))
		b.WriteByte('h')
	}
	if minutes > 0 {
		if hours > 0 {
			b.WriteByte(' ')
		}
		b.WriteString(strconv.Itoa(int(minutes)))
		b.WriteString("min")
	}
	return b.String()
}

func EvalResolution(w int32, h int32) string {
	if w > 0 && h > 0 {
		return strconv.Itoa(int(w)) + "x" + strconv.Itoa(int(h))
	}
	return ""
}
