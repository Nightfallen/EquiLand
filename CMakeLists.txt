# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
cmake_minimum_required (VERSION 3.8)
project ("EquiLand")

# Due to 'https://docs.microsoft.com/en-us/cpp/build/reference/std-specify-language-standard-version?view=msvc-170'
# '/std:c++20' doesn't include 'std::format', '<chrono> formatting extensions', '<ranges> factories and adaptors'
# So temporarily using a little workaround for msvc
if (MSVC)
	if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29.00000)
		add_compile_options(/std:c++latest)
	elseif (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.31.00000)
		if (CMAKE_VERSION VERSION_GREATER 3.20.3)
			set(CMAKE_CXX_STANDARD_REQUIRED True)
			set(CMAKE_CXX_STANDARD 23)
		else()
			add_compile_options(/std:c++latest)
		endif()
	endif()
else()
	set(CMAKE_CXX_STANDARD_REQUIRED True)
	set(CMAKE_CXX_STANDARD 20)
endif()

# Include sub-projects.
add_subdirectory("EquiLand")
add_subdirectory("libpeval")

include(CTest)
if(BUILD_TESTING)
	add_subdirectory("Tests")
endif()