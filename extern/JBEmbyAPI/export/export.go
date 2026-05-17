/////////////////////////////////////////////////////////////////////////////
// Name:        Export.go
// Purpose:     cgo export functions
// Author:      Jan Buchholz
// Created:     2026-04-16
// Last update: 2026-05-14
/////////////////////////////////////////////////////////////////////////////

package main

/*
#include <stdlib.h>
#include <stdbool.h>
*/
import "C"
import (
	"JBEmbyAPI/API"
	"encoding/json"
	"unsafe"
)

//export SendNetworkBroadcast
func SendNetworkBroadcast() {
	API.SendNetworkBroadcast()
}

//export UserLoginToEmbyServer
func UserLoginToEmbyServer(secure C.bool, hostname *C.char, port *C.char,
	username *C.char, password *C.char) *C.char {
	gSecure := bool(secure)
	gHostname := C.GoString(hostname)
	gPort := C.GoString(port)
	gUsername := C.GoString(username)
	gPassword := C.GoString(password)
	result := API.UserLoginToServer(gSecure, gHostname, gPort, gUsername, gPassword)
	return C.CString(jsonReturnResult(result))
}

//export UserGetViews
func UserGetViews(baseurl *C.char, userid *C.char, accesstoken *C.char) *C.char {
	gBaseUrl := C.GoString(baseurl)
	gUserId := C.GoString(userid)
	gAccessToken := C.GoString(accesstoken)
	result := API.UserGetViews(gBaseUrl, gUserId, gAccessToken)
	return C.CString(jsonReturnResult(result))
}

//export UserGetMovies
func UserGetMovies(baseurl *C.char, collectionid *C.char, userid *C.char, accesstoken *C.char) *C.char {
	gBaseUrl := C.GoString(baseurl)
	gCollectionId := C.GoString(collectionid)
	gUserId := C.GoString(userid)
	gAccessToken := C.GoString(accesstoken)
	result := API.UserGetMovies(gBaseUrl, gCollectionId, gUserId, gAccessToken)
	return C.CString(jsonReturnResult(result))
}

//export UserGetSeries
func UserGetSeries(baseurl *C.char, collectionid *C.char, userid *C.char, accesstoken *C.char) *C.char {
	gBaseUrl := C.GoString(baseurl)
	gCollectionId := C.GoString(collectionid)
	gUserId := C.GoString(userid)
	gAccessToken := C.GoString(accesstoken)
	result := API.UserGetSeries(gBaseUrl, gCollectionId, gUserId, gAccessToken)
	return C.CString(jsonReturnResult(result))
}

//export UserGetHomeVideos
func UserGetHomeVideos(baseurl *C.char, collectionid *C.char, userid *C.char, accesstoken *C.char) *C.char {
	gBaseUrl := C.GoString(baseurl)
	gCollectionId := C.GoString(collectionid)
	gUserId := C.GoString(userid)
	gAccessToken := C.GoString(accesstoken)
	result := API.UserGetHomeVideos(gBaseUrl, gCollectionId, gUserId, gAccessToken)
	return C.CString(jsonReturnResult(result))
}

//export UserGetMusicVideos
func UserGetMusicVideos(baseurl *C.char, collectionid *C.char, userid *C.char, accesstoken *C.char) *C.char {
	gBaseUrl := C.GoString(baseurl)
	gCollectionId := C.GoString(collectionid)
	gUserId := C.GoString(userid)
	gAccessToken := C.GoString(accesstoken)
	result := API.UserGetMusicVideos(gBaseUrl, gCollectionId, gUserId, gAccessToken)
	return C.CString(jsonReturnResult(result))
}

//export UserGetMusic
func UserGetMusic(baseurl *C.char, collectionid *C.char, userid *C.char, accesstoken *C.char) *C.char {
	gBaseUrl := C.GoString(baseurl)
	gCollectionId := C.GoString(collectionid)
	gUserId := C.GoString(userid)
	gAccessToken := C.GoString(accesstoken)
	result := API.UserGetMusic(gBaseUrl, gCollectionId, gUserId, gAccessToken)
	return C.CString(jsonReturnResult(result))
}

//export GetPrimaryImageForItem
func GetPrimaryImageForItem(baseurl *C.char, itemid *C.char, imageformat *C.char, imagetag *C.char,
	maxwidth C.int, maxheight C.int, accesstoken *C.char, export C.bool) *C.char {
	gBaseUrl := C.GoString(baseurl)
	gItemId := C.GoString(itemid)
	gImageFormat := C.GoString(imageformat)
	gImageTag := C.GoString(imagetag)
	gMaxWidth := int(maxwidth)
	gMaxHeight := int(maxheight)
	gAccessToken := C.GoString(accesstoken)
	gExport := bool(export)
	result := API.GetPrimaryImageForItem(gBaseUrl, gItemId, gImageFormat, gImageTag,
		gMaxWidth, gMaxHeight, gAccessToken, gExport)
	return C.CString(jsonReturnResult(result))
}

//export FreeString
func FreeString(str *C.char) {
	C.free(unsafe.Pointer(str))
}

func jsonReturnResult(result any) string {
	jsonBytes, _ := json.Marshal(result)
	return string(jsonBytes)
}

func main() {}
