cmake_minimum_required (VERSION 3.20)

add_library(libpeval INTERFACE)
target_include_directories(libpeval INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(libpeval INTERFACE "Constants.hpp" "Hand.hpp" "helpers.hpp")