# -D BOOST_ROOT=/Users/bl/ClionProjects/boost_1_59_0/ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
#-D BOOST_ROOT=/Users/bl/ClionProjects/boost_1_59_0/ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON VERBOSE=1

cmake_minimum_required(VERSION 3.3)
project(ux)







#include(FeatureSummary)

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wno-deprecated-declarations") #-std=c++14 -stdlib=libstdc++

# The version number.
set(BUILD_VERSION_MAJOR 1)
set(BUILD_VERSION_MINOR 0)
set(BUILD_VERSION_MICRO 0)

string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
set(PACKAGE "ux_package")
set(PACKAGE_NAME "ux_package")
set(PACKAGE_VERSION "${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_MICRO}")
set(PACKAGE_STRING "ux_package ${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_MICRO}")
set(PACKAGE_TARNAME "ux_package")
set(PACKAGE_BUGREPORT "bl@ux_package.com")
set(PACKAGE_URL "http://www.ux_package.com/") # empty leaves it undefined
set(VERSION "${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_MICRO}")
set(HEX_VERSION "0x0${BUILD_MAJOR}0${BUILD_MINOR}0${BUILD_MICRO}")

SET(BOOST_ROOT_PATH "${PROJECT_SOURCE_DIR}/../boost_1_59_0" CACHE PATH "BOOST_ROOT")
SET(BOOST_ROOT "${BOOST_ROOT_PATH}")

option (CMAKE_EXPORT_COMPILE_COMMANDS "CMAKE_EXPORT_COMPILE_COMMANDS" ON)
MESSAGE( STATUS "UCMAKE_EXPORT_COMPILE_COMMANDS:         " ${CMAKE_EXPORT_COMPILE_COMMANDS} )

option (VERBOSE "VERBOSE" ON)
MESSAGE( STATUS "VERBOSE:         " ${VERBOSE} )

option (USE_SDL "Use SDL" ON)
MESSAGE( STATUS "Use SDL:         " ${USE_SDL} )

option (USE_IN "Use input wrapper" ON)
MESSAGE( STATUS "USE_IN:         " ${USE_IN} )

option (USE_GL "Use OpenGL" ON)
MESSAGE( STATUS "USE_GL:         " ${USE_GL} )

if (USE_SDL)
endif (USE_SDL)

if (USE_IN)
endif (USE_IN)

if (USE_GL)
endif (USE_GL)


set(CMAKE_MACOSX_RPATH 1) # because of sdl

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
#   find_package(Boost 1.36.0 COMPONENTS date_time filesystem system ...)
find_package(Boost REQUIRED filesystem system)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    set (EXTRA_LIBS ${EXTRA_LIBS} ${Boost_LIBRARIES})
endif()

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
set(EXTRA_LIBS ${EXTRA_LIBS} ${OPENGL_LIBRARIES} )

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})
set(EXTRA_LIBS ${EXTRA_LIBS} ${GLUT_LIBRARIES} )

#PNG_INCLUDE_DIRS, where to find png.h, etc.
#PNG_LIBRARIES, the libraries to link against to use PNG.
#PNG_DEFINITIONS - You should add_definitons(${PNG_DEFINITIONS}) before compiling code that includes png library files.
#PNG_FOUND, If false, do not try to use PNG.
#PNG_VERSION_STRING - the version of the PNG library found (since CMake 2.8.8)
#Also defined, but not for general use are
#
#PNG_LIBRARY, where to find the PNG library.

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
set(EXTRA_LIBS ${EXTRA_LIBS} ${PNG_LIBRARY} )

Find_Package ( SDL REQUIRED )
#Find_Package ( SDL_image REQUIRED ) # if using SDL_image
include_directories(${SDL_INCLUDE_DIR})
set(SOURCE_FILES ${SOURCE_FILES} SDLmain )
set(EXTRA_LIBS ${EXTRA_LIBS} ${SDL_LIBRARY} )

if(sdl2_used) # not complete
    add_subdirectory(SDL2-2.0.3)
    include_directories(SDL2-2.0.3/include)
endif(sdl2_used)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
        "ux_config.h.in"
        "${PROJECT_BINARY_DIR}/ux_config.h"
)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

include_directories(ux_lib)
include_directories(ux_lib/geo)
include_directories(ux_lib/gp)
include_directories(ux_lib/in)

add_subdirectory(ux_lib)
add_subdirectory(ux_tests)


set(MY_SOURCE_FILES main.cpp ux_lib/SDLMain.m)
add_executable(ux_run ${MY_SOURCE_FILES})
target_link_libraries(ux_run ux_lib ${EXTRA_LIBS})

message(STATUS "---------------UX-------------------")
get_property(dirs DIRECTORY . PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dirux='${dir}'")
endforeach()
message(STATUS "------------------------------------")
