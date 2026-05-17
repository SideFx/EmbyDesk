@echo off
REM --- Build Go DLL ---
go env -w CGO_ENABLED=1
go build -buildmode=c-shared -o export/jbembyapi.dll ./export

REM --- Configure CMake (only needed once) ---
cmake -S ./cpp-client -B build

REM --- Clean previous build ---
cmake --build build --target clean

REM --- Build C++ project ---
cmake --build build

REM --- Run ---
build\EmbyAPIDemo.exe -https=(true|false) -host=(hostname or ip-address) -port=8096 -user=(username) -pass=(password)

