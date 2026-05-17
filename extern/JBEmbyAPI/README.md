### JBEmbyAPI – Emby Client Library (Go + C++)

A cross‑platform client library for interacting with an Emby media server.
The project provides a native Go API as well as a C‑compatible shared library that can be consumed from C++ via cgo.

📦 **Features**

**Supported Media Types:**
- Movies (Movies, Folders)
- Series  (Series, Seasons, Episodes)
- Home Videos  (Videos, Folders)
- Music Videos  (Videos, Folders)
- Music  (Albums, Tracks)

**Technical Architecture**
- Core library written in Go
- Idiomatic Go data structures
- JSON parsing
- HTTP communication
- C++ integration via cgo
- exported Go functions using C‑ABI
- shared library generation (DLL / SO / DYLIB)
- C++ wrapper layer for convenient usage

**Demo Applications**
- Go Demo  (go-client/go-client.go)
Demonstrates direct usage of the Go API with command‑line parameters.
- C++ Demo  (cpp-client/main.cpp)
Loads the Go shared library dynamically and uses the generated C++ data structures and JSON parsing helpers.

🛠 **Build & Platform Information**
- Go Library
Build mode: -buildmode=c-shared
- Output:
Windows: JBEmbyAPI.dll + JBEmbyAPI.h
Linux: libJBEmbyAPI.so
macOS: libJBEmbyAPI.dylib
- Requires: CGO_ENABLED=1
- C++ Client
Requires C++20
- Uses nlohmann::json for JSON parsing
- UTF‑8 console output enabled on Windows (if available)

🙏 **Credits**
- Emby Team  
For providing the API structures and documentation.
- Niels Lohmann (nlohmann)  
For the excellent single‑header JSON library json.hpp.
- Microsoft Copilot  
For assisting with generating the C++ data structures from the Go DTOs,
creating the C++ JSON parser functions, and producing the template for loading the Go shared library from C++.

#### Updates

**2026-04-26:**
- Added a `SendNetworkBroadcast` function to trigger the macOS local network permission popup. This function should be called during UI initialization from the host application (currently untested, as no UI exists yet).
- Added FolderData support for Movies.

**2026-05-01:**
- Sort folders by Id
- Return raw numeric values for date and size fields; all formatting and presentation should be handled by the UI layer to ensure correct sorting and localization.

**2026-05-02:**
- Minor fixes (return "Type" for Movies, HomeVideos and MusicVideos)

**2026-05-04:**
- Added DiscNumber for audio tracks
