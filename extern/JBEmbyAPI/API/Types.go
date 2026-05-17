/////////////////////////////////////////////////////////////////////////////
// Name:        Types.go
// Purpose:     Data types for JBEmbyAPI
// Author:      Jan Buchholz
// Created:     2026-04-13
/////////////////////////////////////////////////////////////////////////////

package API

import "net/http"

type ErrorStruct struct {
	Code    int    `json:"Code"`
	Message string `json:"Message"`
}

type embyBaseUrl struct {
	BaseUrl string
	Result  ErrorStruct
}

type embyUserId struct {
	Id     string
	Result ErrorStruct
}

type embyAccessToken struct {
	Token  string
	Result ErrorStruct
}

type EmbySession struct {
	BaseUrl     string `json:"BaseUrl"`
	UserId      string `json:"UserId"`
	AccessToken string `json:"AccessToken"`
}
type EmbyLogonResultExp struct {
	APIVersion string      `json:"ApiVersion"`
	Session    EmbySession `json:"Session"`
	Result     ErrorStruct `json:"Result"`
}

type UserView struct {
	Name           string `json:"Name"`
	CollectionType string `json:"CollectionType"`
	Id             string `json:"Id"`
}

type UserViewsExp struct {
	UserViews []UserView  `json:"UserViews"`
	Result    ErrorStruct `json:"Result"`
}

type UserItems struct {
	Items  []BaseItemDto
	Result ErrorStruct
}

type ItemImageExp struct {
	ItemId    string
	ImageData []byte
	Result    ErrorStruct
}

type embyAuthBody struct {
	Username string
	Pw       string
}

type rESTParams[T any] struct {
	url      string
	response *http.Response
	error    ErrorStruct
	body     []byte
	data     T
}
