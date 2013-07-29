cmake_minimum_required(VERSION 2.8.11.2)
project(Musec)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_COMPILER_IS_CLANG TRUE)
endif()
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
    set(WARNING_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS "-std=c++0x ${WARNING_FLAGS} ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "${WARNING_FLAGS} ${CMAKE_C_FLAGS}")
endif()

# Get the version number from git
set(Musec_VERSION "1.0-untracked")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    find_program(GIT_EXECUTABLE NAMES git git.cmd)
    mark_as_advanced(GIT_EXECUTABLE)
    if(GIT_EXECUTABLE)
        execute_process(COMMAND ${GIT_EXECUTABLE} describe --dirty
                        OUTPUT_VARIABLE Musec_VERSION
                        OUTPUT_STRIP_TRAILING_WHITESPACE
                        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    endif()
endif()
add_definitions(-DMUSEC_VERSION="${Musec_VERSION}")

add_subdirectory(src)
