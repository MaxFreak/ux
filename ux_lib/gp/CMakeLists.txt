
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gp)
#include_directories(${CMAKE_SOURCE_DIR})

MESSAGE( STATUS "USE_GL:         " ${USE_GL} )

if (USE_GL)
    set(GL_SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/gp_wrap_gl.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/gp_wrap_ogl.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/gp_wrap_ogl_sdl.cpp
            )

    set(GL_HEADER_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/gp_wrap_gl.h
            ${CMAKE_CURRENT_SOURCE_DIR}/gp_wrap_ogl.h
            ${CMAKE_CURRENT_SOURCE_DIR}/gp_wrap_ogl_sdl.h
            )
endif (USE_GL)

set(GP_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/gp_wrap.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gp_image_data.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gp_image_decoder.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gp_image_decoder_png.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gp_image_loader.cpp
        ${GL_SOURCE_FILES}
        PARENT_SCOPE)

set(GP_HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/gp_image_data.h
        ${CMAKE_CURRENT_SOURCE_DIR}/gp_image_decoder.h
        ${CMAKE_CURRENT_SOURCE_DIR}/gp_image_decoder_png.h
        ${CMAKE_CURRENT_SOURCE_DIR}/gp_image_loader.h
        ${GL_HEADER_FILES}
        PARENT_SCOPE)

message(STATUS "---------------GP-------------------")
get_property(dirs DIRECTORY . PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dirgp='${dir}'")
endforeach()
message(STATUS "------------------------------------")

