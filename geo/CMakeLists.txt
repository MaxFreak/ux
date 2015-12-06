

set(GEO_SOURCE_FILES
        PARENT_SCOPE)

set(GEO_HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/geo_point.h
        ${CMAKE_CURRENT_SOURCE_DIR}/geo_rectangle.h
        PARENT_SCOPE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/geo)

