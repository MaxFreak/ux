include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories ("${CMAKE_SOURCE_DIR}/GSL")

add_executable(runTests
        basic_check.cpp
        ux_resources_check.cpp
        ux_check.cpp)

target_link_libraries(runTests gtest gtest_main gmock gmock_main)
target_link_libraries(runTests ux_lib)
target_link_libraries(runTests ${EXTRA_LIBS})
