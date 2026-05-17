/////////////////////////////////////////////////////////////////////////////
// Name:        Private.go
// Purpose:     Private functions
// Author:      Jan Buchholz
// Created:     2025-04-15
// Last update: 2026-05-14
/////////////////////////////////////////////////////////////////////////////

package API

import (
	"bytes"
	"encoding/json"
	"io"
	"net"
	"net/http"
	"os"
	"runtime"
	"sort"
	"strconv"
	"strings"
)

func sendNetworkBroadcast() {
	addr, err := net.ResolveUDPAddr("udp4", "255.255.255.255:9999")
	if err != nil {
		return
	}
	conn, err := net.DialUDP("udp4", nil, addr)
	if err != nil {
		return
	}
	defer func() { _ = conn.Close() }()
	_, err = conn.Write([]byte("calling all stations"))
}

func createBasicURL(secure bool, hostname string, port string) embyBaseUrl {
	if hostname == "" {
		return embyBaseUrl{BaseUrl: "", Result: MissingHostname}
	}
	if port == "" {
		return embyBaseUrl{BaseUrl: "", Result: MissingPort}
	}
	var protocol string
	if secure {
		protocol = ProtocolHttps
	} else {
		protocol = ProtocolHttp
	}
	return embyBaseUrl{BaseUrl: protocol + "://" + hostname + ":" + port + "/emby", Result: NoError}
}

func findUserIdByName(baseurl string, username string) embyUserId {
	p := rESTParams[[]UserDto]{}
	p.url = baseurl + getUsersPublic
	p.genericHttpGet()
	if p.error.Code != NoErrorConst {
		return embyUserId{Id: "", Result: p.error}
	}
	for _, user := range p.data {
		if strings.EqualFold(user.Name, username) {
			if !user.HasConfiguredPassword {
				return embyUserId{Id: user.Id, Result: UserPasswordError}
			}
			return embyUserId{Id: user.Id, Result: NoError}
		}
	}
	return embyUserId{Id: "", Result: UserNotFound}
}

func authenticateUserByCredentials(baseurl string, userid string, username string, password string) embyAccessToken {
	authBody := embyAuthBody{username, password}
	jBody, err := json.Marshal(authBody)
	if err != nil {
		return embyAccessToken{Token: "", Result: JsonError}
	}
	url := baseurl + postAuthenticateUser
	req, err := http.NewRequest(http.MethodPost, url, bytes.NewBuffer(jBody))
	if err != nil {
		return embyAccessToken{Token: "", Result: HttpPostFailed}
	}
	req.Header.Add(contentType, contentTypeJSON)
	req.Header.Add(authHeader, createHeader(userid))
	authClient := &http.Client{}
	response, err := authClient.Do(req)
	defer func() { _ = response.Body.Close() }()
	if err != nil {
		return embyAccessToken{Token: "", Result: HttpPostFailed}
	}
	if response.StatusCode != http.StatusOK {
		return embyAccessToken{Token: "", Result: AuthenticationError}
	}
	resBody, err := io.ReadAll(response.Body)
	defer func() { _ = io.ReadCloser.Close(response.Body) }()
	if err != nil {
		return embyAccessToken{Token: "", Result: IoError}
	}
	var authResult AuthenticationResult
	err = json.Unmarshal(resBody, &authResult)
	if err != nil {
		return embyAccessToken{Token: "", Result: JsonError}
	}
	return embyAccessToken{Token: authResult.AccessToken, Result: NoError}
}

func userGetItems(baseurl string, collectionid string,
	collectiontype string, userid string, accesstoken string) UserItems {
	var items UserItems
	p := rESTParams[QueryResultBaseItemDto]{}
	p.url = baseurl + getUserItems
	p.url = strings.Replace(p.url, "&1", userid, 1)
	p.url += "?" + paraApiKey + accesstoken
	p.url += "&" + paraRecursive + "true"
	p.url += "&" + paraParentId + collectionid
	p.url += "&" + paraIncludeItemTypes + getIncludeItemTypes(collectiontype)
	p.url += "&" + paraFields + getAPIFields(collectiontype)
	p.genericHttpGet()
	if p.error.Code != NoErrorConst {
		items.Result = p.error
		return items
	}
	for _, item := range p.data.Items {
		items.Items = append(items.Items, item)
	}
	return items
}

func checkCollectionType(baseurl string, userid string, accesstoken string, collectionid string,
	collectiontype string) ErrorStruct {
	views := UserGetViews(baseurl, userid, accesstoken)
	if views.Result.Code != NoErrorConst {
		return views.Result
	}
	var b = false
	for _, view := range views.UserViews {
		if view.Id == collectionid {
			b = true
			if view.CollectionType != collectiontype {
				return WrongCollectionType
			}
		}
	}
	if !b {
		return CollectionNotFound
	}
	return NoError
}

func (p *rESTParams[T]) genericHttpGet() {
	var err error
	if p.url == "" {
		p.error = ParameterError
		return
	}
	p.response, err = http.Get(p.url)
	if err != nil {
		p.error = HttpGetFailed
		return
	}
	defer func() { _ = p.response.Body.Close() }()
	if p.response.StatusCode != http.StatusOK {
		e := HttpStatusError
		e.Message = strings.Replace(e.Message, "&1", strconv.Itoa(p.response.StatusCode), -1)
		p.error = e
		return
	}
	p.body, err = io.ReadAll(p.response.Body)
	if err != nil {
		p.error = IoError
		return
	}
	err = json.Unmarshal(p.body, &p.data)
	if err != nil {
		p.error = JsonError
		return
	}
	p.error = NoError
}

func createPair(key string, value string) string {
	const qu = `"`
	return key + "=" + qu + value + qu
}

func createHeader(userid string) string {
	var h string
	host, _ := os.Hostname()
	h = authType + " " +
		createPair(authKeyUserId, userid) + ", " +
		createPair(authKeyClient, authClient) + ", " +
		createPair(authKeyDevice, runtime.GOOS) + ", " +
		createPair(authKeyDeviceId, host) + ", " +
		createPair(authKeyVersion, "1.0.0.0")
	return h
}

func getAPIFields(collectiontype string) string {
	switch collectiontype {
	case CollectionMovies:
		return strings.Join(MovieTable.APIFields, ",")
	case CollectionSeries:
		return strings.Join(SeriesTable.APIFields, ",")
	case CollectionHomeVideos:
		return strings.Join(HomeVideoTable.APIFields, ",")
	case CollectionMusicVideos:
		return strings.Join(MusicVideoTable.APIFields, ",")
	case CollectionMusic:
		return strings.Join(MusicTable.APIFields, ",")
	default:
		return ""
	}
}

func getIncludeItemTypes(collectiontype string) string {
	switch collectiontype {
	case CollectionMovies:
		return strings.Join(MovieTable.IncludeItemTypes, ",")
	case CollectionSeries:
		return strings.Join(SeriesTable.IncludeItemTypes, ",")
	case CollectionHomeVideos:
		return strings.Join(HomeVideoTable.IncludeItemTypes, ",")
	case CollectionMusicVideos:
		return strings.Join(MusicVideoTable.IncludeItemTypes, ",")
	case CollectionMusic:
		return strings.Join(MusicTable.IncludeItemTypes, ",")
	default:
		return ""
	}
}

func evalNameLongIdPairs(pairs []NameLongIdPair) []string {
	names := make([]string, 0, len(pairs))
	for _, p := range pairs {
		names = append(names, p.Name)
	}
	return names
}

func evalNameIdPairs(pairs []NameIdPair) []string {
	names := make([]string, 0, len(pairs))
	for _, p := range pairs {
		names = append(names, p.Name)
	}
	return names
}

func evalPersons(persons []BaseItemPerson, types ...string) []string {
	personsFound := make([]string, 0, len(persons))
	for _, p := range persons {
		for _, t := range types {
			if p.Type == t {
				personsFound = append(personsFound, p.Name)
			}
		}
	}
	return personsFound
}

func evalAlbumArtists(pair []NameIdPair) (string, string) {
	if len(pair) == 0 {
		return "", ""
	}
	return pair[0].Name, pair[0].Id
}

func evalCodecs(media []MediaSourceInfo) (string, string) {
	audioSet := make(map[string]struct{})
	videoSet := make(map[string]struct{})
	for _, m := range media {
		for _, s := range m.MediaStreams {
			switch s.Type {
			case AudioMediaStreamType:
				audioSet[s.Codec] = struct{}{}
			case VideoMediaStreamType:
				videoSet[s.Codec] = struct{}{}
			}
		}
	}
	// Sets -> Slices
	audio := make([]string, 0, len(audioSet))
	for k := range audioSet {
		audio = append(audio, k)
	}
	video := make([]string, 0, len(videoSet))
	for k := range videoSet {
		video = append(video, k)
	}
	sort.Strings(audio)
	sort.Strings(video)
	return strings.Join(audio, ", "), strings.Join(video, ", ")
}

func sortFoldersById(folders []FolderDataInc) {
	sort.Slice(folders, func(i, j int) bool {
		return folders[i].FolderId < folders[j].FolderId
	})
}
