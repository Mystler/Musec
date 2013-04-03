cmake_minimum_required(VERSION 2.8.9)
project(Musec)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)

set(HEADERS
    Musec.h
)

set(CPPFILES
    main.cpp
    Musec.cpp
)

set(RESOURCES
    res/musec.rc
)

set(QT_FORMS
    res/musec.ui
)

set(QT_RESOURCES
    res/musec.qrc
)

qt5_wrap_ui(Musec_UI ${QT_FORMS})
qt5_add_resources(Musec_RES ${QT_RESOURCES})

add_executable(Musec WIN32 ${HEADERS} ${CPPFILES} ${RESOURCES}
    ${Musec_UI} ${Musec_RES})

target_link_libraries(Musec Qt5::WinMain)

qt5_use_modules(Musec Widgets Multimedia)
