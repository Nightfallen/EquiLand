# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("EquiLand")

set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)

# Include sub-projects.
add_subdirectory("EquiLand")
add_subdirectory("libpeval")

include(CTest)
if(BUILD_TESTING)
	add_subdirectory("Tests")
endif()