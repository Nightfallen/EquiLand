cmake_minimum_required (VERSION 3.8)

# GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
#include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Unit Tests
# Add test cpp file
add_executable( EquiLandTests "tests_app.cpp")
target_link_libraries(EquiLandTests ${GTEST_BOTH_LIBRARIES})
target_link_libraries(EquiLandTests )
add_test( NAME EquiLandTests COMMAND EquiLandTests )

# library tests
add_executable( LibpevalTests "tests_libpeval.cpp")
target_link_libraries(LibpevalTests PRIVATE ${GTEST_BOTH_LIBRARIES})
target_link_libraries(LibpevalTests PRIVATE libpeval)
add_test( NAME LibpevalTests COMMAND LibpevalTests )
