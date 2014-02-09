find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

set(Musec_HEADERS
    Musec.h
    Global.h
    NetMgr.h
    Score.h
)
set(Musec_SOURCES
    main.cpp
    Musec.cpp
    NetMgr.cpp
    Score.cpp
)
configure_file(../res/locales.qrc.in locales.qrc)
set(Musec_QRC ../res/musec.qrc ${CMAKE_CURRENT_BINARY_DIR}/locales.qrc)
set(Musec_QUI ../res/musec.ui)
set(Musec_TS
    ../loc/musec_de.ts
    ../loc/musec_fr.ts
)

qt5_wrap_ui(Musec_UI ${Musec_QUI})
qt5_add_resources(Musec_RES ${Musec_QRC})
qt5_add_translation(Musec_LOC ${Musec_TS})

if(WIN32)
    set(Musec_RC ../res/musec.rc)
endif()

add_executable(Musec WIN32 MACOSX_BUNDLE
    ${Musec_HEADERS} ${Musec_SOURCES}
    ${Musec_UI} ${Musec_RES}
    ${Musec_RC} ${Musec_LOC}
)

target_link_libraries(Musec Qt5::Widgets Qt5::Multimedia Qt5::Network)

source_group("Header Files" FILES ${Musec_HEADERS})
source_group("Source Files" FILES ${Musec_SOURCES})
source_group("Resource Files" FILES
    ${Musec_QUI} ${Musec_QRC} ${Musec_RC}
)
source_group("Generated Files" FILES
    ${Musec_UI} ${Musec_RES} ${Musec_LOC}
)

install(TARGETS Musec DESTINATION .)
