# CMakeList.txt : CMake project for EquiLand, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (EquiLand "EquiLand.cpp" "EquiLand.h" "Settings.hpp")

find_package(fmt)
target_link_libraries(EquiLand PRIVATE fmt::fmt)
target_link_libraries(EquiLand PRIVATE libpeval)