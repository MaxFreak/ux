

set(GEO_SOURCE_FILES
        PARENT_SCOPE)

set(GEO_HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/geo_point.h
        ${CMAKE_CURRENT_SOURCE_DIR}/geo_rectangle.h
        PARENT_SCOPE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/geo)

message(STATUS "----------------GEO----------------")
get_property(dirs DIRECTORY . PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dirgeo='${dir}'")
endforeach()
message(STATUS "------------------------------------")
