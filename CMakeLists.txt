cmake_minimum_required(VERSION 3.19)

set(PROJECT_NAME "EmbyDesk")
project(${PROJECT_NAME} LANGUAGES CXX)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_NO_PRIVATE_MODULE_WARNING ON)
find_package(Qt6 6.11 REQUIRED COMPONENTS Core Widgets Svg Sql GuiPrivate)

qt_standard_project_setup()
add_subdirectory(extern/JBEmbyAPI/cpp-client/)
add_subdirectory(extern/QXlsx/QXlsx/)

qt_add_executable(${PROJECT_NAME}
    WIN32 MACOSX_BUNDLE
    MANUAL_FINALIZATION
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    conversions.hpp
    app.icns app.ico appicon.rc
    jbstyles.hpp
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
    Info.plist.in

)

# Show below files in the QtCreator project tree
target_sources(${PROJECT_NAME} PRIVATE
    extern/JBEmbyAPI/cpp-client/jbgolib.hpp
    extern/JBEmbyAPI/cpp-client/jbparser.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${JBEmbyAPI_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR} #include <jbfontcombobox.hpp>
)

target_link_libraries(${PROJECT_NAME}
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
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INSTALL_RPATH "@executable_path/../Frameworks"
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )
endif()

add_dependencies(${PROJECT_NAME} JBEmbyAPI)

if(WIN32)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${JBEmbyAPI_LIB}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
endif()

install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(APPLE)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/debug_entitlements.plist
                "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://apple.com\">\n"
                "<plist version=\"1.0\">\n"
                "<dict>\n"
                "    <key>com.apple.security.get-task-allow</key>\n"
                "    <true/>\n"
                "</dict>\n"
                "</plist>\n"
    )
    get_filename_component(QT_BIN_DIR "${Qt6_DIR}/../../../bin" ABSOLUTE)
    find_program(MACDEPLOYQT macdeployqt6 HINTS "${QT_BIN_DIR}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Frameworks"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${JBEmbyAPI_LIB}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Frameworks/"
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${MACDEPLOYQT} "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>" -verbose=2
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND codesign --force --deep --sign - --timestamp=none "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>"
        COMMAND codesign --force --sign - --timestamp=none --entitlements
            ${CMAKE_CURRENT_BINARY_DIR}/debug_entitlements.plist "$<TARGET_FILE:${PROJECT_NAME}>"
        )
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

