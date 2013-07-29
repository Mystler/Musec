find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

set(Musec_HEADERS
    Musec.h
    Score.h
)
set(Musec_SOURCES
    main.cpp
    Musec.cpp
    Score.cpp
)
set(Musec_QRC ../res/musec.qrc)
set(Musec_QUI ../res/musec.ui)
set(Musec_TS
    ../loc/musec_de.ts
    ../loc/musec_fr.ts
)

qt5_wrap_ui(Musec_UI ${Musec_QUI})
qt5_wrap_cpp(Musec_MOC ${Musec_HEADERS})
qt5_add_resources(Musec_RES ${Musec_QRC})
qt5_add_translation(Musec_LOC ${Musec_TS})
if(WIN32)
    set(Musec_RC ../res/musec.rc)
endif()

add_executable(Musec WIN32 MACOSX_BUNDLE
    ${Musec_MOC} ${Musec_SOURCES}
    ${Musec_UI} ${Musec_RES}
    ${Musec_RC} ${Musec_LOC}
)

qt5_use_modules(Musec Widgets Multimedia)

source_group("Source Files" FILES ${Musec_SOURCES})
source_group("Resource Files" FILES
    ${Musec_QUI} ${Musec_QRC} ${Musec_RC}
)
source_group("Generated Files" FILES
    ${Musec_MOC} ${Musec_RES} ${Musec_UI} ${Musec_LOC}
)

install(TARGETS Musec DESTINATION .)
install(FILES ${Musec_LOC} DESTINATION locales)
