cmake_minimum_required(VERSION 2.8.9)
project(Musec)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)

set(Musec_HEADERS
    Musec.h
)
set(Musec_SOURCES
    main.cpp
    Musec.cpp
)
set(Musec_QRC res/musec.qrc)
set(Musec_QUI res/musec.ui)

qt5_wrap_ui(Musec_UI ${Musec_QUI})
qt5_add_resources(Musec_RES ${Musec_QRC})
qt5_wrap_cpp(Musec_MOC ${Musec_HEADERS})
if(WIN32)
    set(Musec_RES ${Musec_RES} res/musec.rc)
endif()

add_executable(Musec WIN32
    ${Musec_MOC} ${Musec_SOURCES}
    ${Musec_UI} ${Musec_RES}
)

target_link_libraries(Musec ${Qt5Core_QTMAIN_LIBRARIES})
qt5_use_modules(Musec Widgets Multimedia)

source_group("Source Files" FILES ${Musec_SOURCES})
source_group("MOC Files" FILES ${Musec_MOC})
source_group("Resource Files" FILES
    ${Musec_RES} ${Musec_UI} ${Musec_QUI} ${Musec_QRC}
)
