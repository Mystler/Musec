project(Musec)
cmake_minimum_required(VERSION 2.8.9)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

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

add_executable(Musec ${CPPFILES} ${RESOURCES}
    ${HEADERS} ${Musec_UI} ${Musec_RES})

qt5_use_modules(Musec Widgets Multimedia)
