cmake_minimum_required(VERSION 3.19)
project(EmbyDesk LANGUAGES CXX)
set(CMAKE_AUTORCC ON)
set(QT_NO_PRIVATE_MODULE_WARNING ON)
find_package(Qt6 6.11 REQUIRED COMPONENTS Core Widgets Svg Sql GuiPrivate)

qt_standard_project_setup()
add_subdirectory(extern/JBEmbyAPI/cpp-client/)
add_subdirectory(extern/QXlsx/QXlsx/)

qt_add_executable(EmbyDesk
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    conversions.hpp
    app.icns app.ico appicon.rc
    stylesheets.h
    globals.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource.qrc
    jbpreferences.cpp jbpreferences.h preferencesdialog.cpp preferencesdialog.h preferencesdialog.ui
    appsettings.h appsettings.cpp
    splashdialog.cpp splashdialog.h splashdialog.ui
    sheets.hpp
    models/movietreemodel.h models/movietreemodel.cpp
    models/homevideotreemodel.h models/homevideotreemodel.cpp
    models/musicvideotreemodel.h models/musicvideotreemodel.cpp
    jbimagecache.h jbimagecache.cpp
    apicall.h apicall.cpp
    models/seriestreemodel.h models/seriestreemodel.cpp
    models/musictreemodel.h models/musictreemodel.cpp
    aboutdialog.cpp aboutdialog.h aboutdialog.ui
    metatypes.h
    models/headertexts.h
    excelexport.h excelexport.cpp
    lookupkeys.h
    embydumpdialog.h embydumpdialog.cpp embydumpdialog.ui
    dumpworker.h dumpworker.cpp
    sqlreader.h sqlreader.cpp
    sqlreturntypes.h
    models/statistics.h
)

# Show below files in the QtCreator project tree
target_sources(EmbyDesk PRIVATE
    extern/JBEmbyAPI/cpp-client/jbgolib.hpp
    extern/JBEmbyAPI/cpp-client/jbparser.hpp
)

target_include_directories(EmbyDesk PRIVATE
    ${JBEmbyAPI_INCLUDE_DIRS}
)

set_target_properties(JBEmbyAPI PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/EmbyDesk.app/Contents/MacOS"
)

target_link_libraries(EmbyDesk
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Svg
        Qt::GuiPrivate
        Qt::Sql
        QXlsx::QXlsx
)

if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE app.icns)
    set(APP_ICON "${CMAKE_SOURCE_DIR}/app.icns")
    set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_dependencies(EmbyDesk JBEmbyAPI)

add_custom_command(TARGET EmbyDesk POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${JBEmbyAPI_LIB}
        $<TARGET_FILE_DIR:EmbyDesk>
)

install(TARGETS EmbyDesk
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
