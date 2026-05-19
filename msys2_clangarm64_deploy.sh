#!/usr/bin/env bash

APP="$1"
DEPLOY=deploy

echo "Deploy started"
echo "App: $APP"

if [ ! -f "$APP" ]; then
    echo "ERROR: EXE not found!"
    exit 1
fi

mkdir -p "$DEPLOY"

echo "Copy EXE..."
cp "$APP" "$DEPLOY/"
echo "Copy DLL..."
cp jbembyapi.dll "$DEPLOY/"

echo "Qt Deployment..."
windeployqt6 --release "$APP" --dir "$DEPLOY"

echo "MSYS2/UCRT64 libraries..."
ldd "$APP" | grep "clangarm64" | awk '{print $3}' | while read dll; do
    if [ -f "$dll" ]; then
        cp -n "$dll" "$DEPLOY/"
    fi
done

echo "Done!"
