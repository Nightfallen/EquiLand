cmake_minimum_required (VERSION 3.8)

# GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
#include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Unit Tests
# Add test cpp file

include_directories("${PROJECT_SOURCE_DIR}/EquiLand")
add_executable( EquiLandTests "tests_app.cpp")
target_precompile_headers(EquiLandTests REUSE_FROM ${PROJECT_NAME})
target_link_libraries(EquiLandTests PRIVATE ${GTEST_BOTH_LIBRARIES})
#target_link_libraries(EquiLandTests PRIVATE ${PROJECT_NAME}_lib) #
add_test( NAME EquiLandTests COMMAND EquiLandTests )

# library tests
add_executable( LibpevalTests "tests_libpeval.cpp")
target_link_libraries(LibpevalTests PRIVATE ${GTEST_BOTH_LIBRARIES})
target_link_libraries(LibpevalTests PRIVATE libpeval)
add_test( NAME LibpevalTests COMMAND LibpevalTests )
