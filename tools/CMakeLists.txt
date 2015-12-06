#IF(ANDROID)
#    SET(MODULE_USER_STATIC_OR_DYNAMIC ${OPENSCENEGRAPH_USER_DEFINED_DYNAMIC_OR_STATIC})
#ENDIF(ANDROID)

#IF(MSVC80 OR MSVC90)
#  OPTION(OSG_MSVC_GENERATE_PLUGINS_AND_WRAPPERS_MANIFESTS "Generate or not manifests files under VS8 for dynamically loaded dlls" ON)
#ENDIF()
#the old construct SUBDIRS( was substituded by ADD_SUBDIRECTORY that is to be preferred according on CMake docs.
#FOREACH( mylibfolder
#        test
#    )
#    #ADD_SUBDIRECTORY(${mylibfolder})
#ENDFOREACH()

set(TOOLS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tools.cpp PARENT_SCOPE)
set(TOOLS_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tools.h PARENT_SCOPE)

#target_include_directories(ux PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tools)

