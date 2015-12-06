

MESSAGE( STATUS "USE_IN:         " ${USE_IN} )

if (USE_IN)
    set(IN_SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/in_device.cpp
#            ${CMAKE_CURRENT_SOURCE_DIR}/in_device_null.cpp
            )

    set(IN_HEADER_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/in_event.h
            ${CMAKE_CURRENT_SOURCE_DIR}/in_device.h
            ${CMAKE_CURRENT_SOURCE_DIR}/in_device_null.h
            )
endif (USE_IN)

if (USE_SDL)
    set(IN_SOURCE_FILES
            ${IN_SOURCE_FILES}
            ${CMAKE_CURRENT_SOURCE_DIR}/in_device_sdl.cpp
            )

    set(IN_HEADER_FILES
            ${IN_HEADER_FILES}
            ${CMAKE_CURRENT_SOURCE_DIR}/in_device_sdl.h
            )
endif (USE_SDL)

set(IN_SOURCE_FILES
        ${IN_SOURCE_FILES}
        PARENT_SCOPE)

set(IN_HEADER_FILES
        ${IN_HEADER_FILES}
        PARENT_SCOPE)
