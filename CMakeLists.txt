# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20)

project ("EquiLand")


set(CMAKE_CXX_STANDARD_REQUIRED True)
if(MSVC)
# Due to 'https://docs.microsoft.com/en-us/cpp/build/reference/std-specify-language-standard-version?view=msvc-170'
# '/std:c++20' doesn't include 'std::format', '<chrono> formatting extensions', '<ranges> factories and adaptors'
# So temporarily using a little workaround for msvc
	set(CMAKE_CXX_STANDARD 23)
else()
	set(CMAKE_CXX_STANDARD 20)
endif()



# Include sub-projects.
add_subdirectory("EquiLand")
add_subdirectory("libpeval")

include(CTest)
if(BUILD_TESTING)
	add_subdirectory("Tests")
endif()