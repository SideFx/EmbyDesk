/////////////////////////////////////////////////////////////////////////////
// Name:        Errors.go
// Purpose:     Error definitions (code & message)
// Author:      Jan Buchholz
// Created:     2026-05-14
/////////////////////////////////////////////////////////////////////////////

package API

const NoErrorConst = 0

var (
	NoError             = ErrorStruct{NoErrorConst, "OK."}
	MissingHostname     = ErrorStruct{1, "Missing hostname."}
	MissingPort         = ErrorStruct{2, "Missing port."}
	HttpGetFailed       = ErrorStruct{3, "HTTP GET failed."}
	HttpPostFailed      = ErrorStruct{4, "HTTP POST failed."}
	HttpStatusError     = ErrorStruct{5, "HTTP status error &1."}
	IoError             = ErrorStruct{6, "I/O error."}
	JsonError           = ErrorStruct{7, "JSON error."}
	UserNotFound        = ErrorStruct{8, "User not found."}
	ParameterError      = ErrorStruct{9, "Missing or invalid parameter error."}
	UserPasswordError   = ErrorStruct{10, "No password configured for user."}
	AuthenticationError = ErrorStruct{11, "Authentication error."}
	WrongCollectionType = ErrorStruct{12, "Selected collected is not of expected type."}
	CollectionNotFound  = ErrorStruct{13, "Collection not found."}
)
