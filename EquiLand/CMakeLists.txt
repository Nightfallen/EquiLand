# CMakeList.txt : CMake project for EquiLand, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(MSVC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29.00000)
# Due to 'https://docs.microsoft.com/en-us/cpp/build/reference/std-specify-language-standard-version?view=msvc-170'
# '/std:c++20' doesn't include 'std::format', '<chrono> formatting extensions', '<ranges> factories and adaptors'
# So temporarily using a little workaround for msvc
	add_compile_options(/std:c++latest)
else(MSVC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.31.00000)
	if (CMAKE_VERSION VERSION_GREATER 3.20.3)
		set(CMAKE_CXX_STANDARD 23)
	else()
		add_compile_options(/std:c++latest)
	endif()
endif()
 
# Add source to this project's executable.
set(		ImGui_Docking_Sources "imgui/imconfig.h" "imgui/imgui.cpp" "imgui/imgui.h" "imgui/imgui_demo.cpp")
list(APPEND ImGui_Docking_Sources "imgui/imgui_draw.cpp" "imgui/imgui_internal.h" "imgui/imgui_tables.cpp")
list(APPEND ImGui_Docking_Sources "imgui/imgui_widgets.cpp" "imgui/imstb_rectpack.h" "imgui/imstb_textedit.h")
list(APPEND ImGui_Docking_Sources "imgui/imstb_truetype.h" "imgui/imgui_impl_dx11.cpp" "imgui/imgui_impl_dx11.h")
list(APPEND ImGui_Docking_Sources "imgui/imgui_impl_win32.cpp" "imgui/imgui_impl_win32.h")
list(APPEND ImGui_Docking_Sources "imgui/imgui_freetype.cpp" "imgui/imgui_freetype.h")
#list(APPEND ImGui_Docking_Sources "imgui/imgui_impl_glfw.cpp" "imgui/imgui_impl_glfw.h")
#list(APPEND ImGui_Docking_Sources "imgui/imgui_impl_opengl3.cpp" "imgui/imgui_impl_opengl3.h" "imgui/imgui_impl_opengl3_loader.h")

set(		OMPEval_Sources "OMPEval/CardRange.cpp" "OMPEval/CardRange.h" "OMPEval/CombinedRange.cpp")
list(APPEND OMPEval_Sources "OMPEval/CombinedRange.h" "OMPEval/Constants.h" "OMPEval/EquityCalculator.cpp")
list(APPEND OMPEval_Sources "OMPEval/EquityCalculator.h" "OMPEval/Hand.h" "OMPEval/HandEvaluator.h")
list(APPEND OMPEval_Sources "OMPEval/HandEvaluator.cpp" "OMPEval/OffsetTable.hxx" "OMPEval/Random.h" "OMPEval/Util.h")

set(		EquiLand_Sources "EquiLand.cpp" "includes_pch.h" "includes_pch.cpp")
list(APPEND EquiLand_Sources "source/structures.hpp" "source/gui.hpp" "source/Freetype.hpp")
list(APPEND EquiLand_Sources "source/Settings.hpp"  "source/Application.h" "source/Application.cpp")
list(APPEND EquiLand_Sources "source/Styles.hpp" "source/Fonts.hpp" "source/Notifications.hpp")
list(APPEND EquiLand_Sources "source/utils/ColorHelpers.hpp" "source/ApplicationMenu.hpp")

set(		EquiLand_Widgets_Sources "source/widgets/BasicCustomWidgets.hpp"	"source/widgets/AnimatedCustomWidgets.hpp")
list(APPEND EquiLand_Widgets_Sources "source/widgets/RangeSelect.hpp"			"source/widgets/IncludeAllWidgets.hpp")
list(APPEND EquiLand_Widgets_Sources "source/widgets/CardSelection.hpp"			"source/widgets/TreeRangeEditor.hpp")
list(APPEND EquiLand_Widgets_Sources "source/widgets/BoardSelect.hpp"			"source/widgets/DeadCardsSelect.hpp")

set(		EquiLand_Fonts_Sources "fonts/DefinesFontAwesome.hpp" "fonts/BinaryFontAwesome.hpp")
list(APPEND EquiLand_Fonts_Sources "fonts/BinaryFontRoboto.hpp")

set(		EquiLand_Project_Sources ${ImGui_Docking_Sources} ${EquiLand_Sources} ${OMPEval_Sources})
list(APPEND EquiLand_Project_Sources ${EquiLand_Fonts_Sources} ${EquiLand_Widgets_Sources})

# disable warnings from imgui
set_source_files_properties(
  ${ImGui_Docking_Sources}
  PROPERTIES
  COMPILE_FLAGS "-w"
)

# LiStarted implementing Notificationsnking all sources
add_executable (EquiLand ${EquiLand_Project_Sources})



# External libraries handling


find_package(libdivide CONFIG REQUIRED)
target_link_libraries(EquiLand PRIVATE libdivide::libdivide)


if (WIN32 OR MSVC)
find_package(freetype CONFIG REQUIRED)
target_link_libraries(EquiLand PRIVATE freetype)

#if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.31.30129)
#find_package(freetype CONFIG REQUIRED)
#target_link_libraries(EquiLand PRIVATE ${FREETYPE_LIBRARIES})
#target_include_directories(EquiLand PRIVATE ${FREETYPE_INCLUDE_DIRS})
#else()
#find_package(Freetype REQUIRED)
#target_link_libraries(EquiLand PRIVATE Freetype::Freetype)
#endif()

endif(WIN32 OR MSVC)

if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif(UNIX AND NOT APPLE)

if (LINUX AND NOT MSVC)
find_package(Freetype REQUIRED)
target_link_libraries(EquiLand PRIVATE Freetype::Freetype)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(EquiLand PRIVATE fmt::fmt-header-only)

find_package(OpenGL REQUIRED)
target_link_libraries(EquiLand PRIVATE OpenGL::GL)
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(EquiLand PRIVATE glfw)

add_compile_options(-I/opt/vcpkg/installed/x64-linux/include)

endif(LINUX)


target_link_libraries(EquiLand PRIVATE libpeval)

# Post-lib project setup
target_precompile_headers(EquiLand PRIVATE "includes_pch.h")
if(MSVC)
	add_definitions(/FI"includes_pch.h")
else()
	# GCC or Clang
	add_definitions(-include includes_pch.h)
endif()