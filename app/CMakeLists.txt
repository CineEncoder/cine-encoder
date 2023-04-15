cmake_minimum_required(VERSION 3.16)
project(cine_encoder VERSION 3.5.5 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up AUTOMOC and some sensible defaults for runtime execution
# When using Qt 6.3, you can replace the code block below with
# qt_standard_project_setup()
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
include(GNUInstallDirs)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Multimedia Svg)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets X11Extras)

add_executable(cine_encoder WIN32 MACOSX_BUNDLE
    basedialog.cpp basedialog.h
    constants.h
    encoder.cpp encoder.h
    encoderstream.cpp encoderstream.h
    fileiconprovider.cpp fileiconprovider.h
    helper.cpp helper.h
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    message.cpp message.h message.ui
    notification.cpp notification.h notification.ui
    popupmessage.cpp popupmessage.h popupmessage.ui
    preset.cpp preset.h preset.ui
    progress.cpp progress.h progress.ui
    report.cpp report.h report.ui
    settings.cpp settings.h settings.ui
    streamconverter.cpp streamconverter.h streamconverter.ui
    tables.h
    widgets/qanimatedsvg.cpp widgets/qanimatedsvg.h
    widgets/qdoublebutton.cpp widgets/qdoublebutton.h
    widgets/qstreamview.cpp widgets/qstreamview.h
    files.qrc
)
target_link_libraries(cine_encoder PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Multimedia
    Qt::Svg
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(cine_encoder PRIVATE
        Qt::Widgets
    )
endif()

if(DEFINES___contains___WM_CUSTOM)
    target_sources(cine_encoder PUBLIC
        basewindow_cwm.cpp basewindow_cwm.h
    )
endif()

if(UNIX AND NOT DEFINES___contains___WM_CUSTOM AND NOT MACOS)
    target_sources(cine_encoder PUBLIC
        platform_unix/basewindow.cpp platform_unix/basewindow.h
        platform_unix/xutil.cpp platform_unix/xutil.h
    )
endif()

if(WIN32 AND NOT DEFINES___contains___WM_CUSTOM)
    target_sources(cine_encoder PUBLIC
        platform_win/basewindow.cpp platform_win/basewindow.h
    )
endif()

if(WIN32)
    target_link_libraries(cine_encoder PRIVATE
        dwmapi
        user32
    )
endif()

if(UNIX AND NOT MACOS)
    target_link_libraries(cine_encoder PRIVATE
        Qt::X11Extras
        X11
        Xext
        mediainfo
    )
endif()

install(TARGETS cine_encoder
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Consider using qt_generate_deploy_app_script() for app deployment if
# the project can use Qt 6.3. In that case rerun qmake2cmake with
# --min-qt-version=6.3.
