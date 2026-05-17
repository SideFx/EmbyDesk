@echo off
cd go-client
go run go-client.go -https=(true|false) -host=(hostname or ip-address) -port=8096 -user=(username) -pass=(password)
cd ..

