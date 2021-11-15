# CMakeList.txt : CMake project for EquiLand, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Add source to this project's executable.
set(ImGui_Docking_Sources "imgui/imconfig.h" "imgui/imgui.cpp" "imgui/imgui.h" "imgui/imgui_demo.cpp")
list(APPEND ImGui_Docking_Sources "imgui/imgui_draw.cpp" "imgui/imgui_internal.h" "imgui/imgui_tables.cpp")
list(APPEND ImGui_Docking_Sources "imgui/imgui_widgets.cpp" "imgui/imstb_rectpack.h" "imgui/imstb_textedit.h")
list(APPEND ImGui_Docking_Sources "imgui/imstb_truetype.h" "imgui/imgui_impl_dx11.cpp" "imgui/imgui_impl_dx11.h")
list(APPEND ImGui_Docking_Sources "imgui/imgui_impl_win32.cpp" "imgui/imgui_impl_win32.h")
list(APPEND ImGui_Docking_Sources "imgui/imgui_freetype.cpp" "imgui/imgui_freetype.h")

set(EquiLand_Sources   "EquiLand.cpp" "includes_pch.h" "includes_pch.cpp")
list(APPEND EquiLand_Sources "source/structures.hpp" "source/gui.hpp" "source/custom_widgets.hpp" "source/Freetype.hpp")
list(APPEND EquiLand_Sources "source/Settings.hpp"  "source/Application.h" "source/Application.cpp")

#add_executable (EquiLand ${ImGui_Docking_Sources})
#add_executable (EquiLand "EquiLand.cpp"  "source/Settings.hpp" "includes_pch.h" "includes_pch.cpp")
add_executable (EquiLand ${ImGui_Docking_Sources} ${EquiLand_Sources})
#set_property(TARGET EquiLand PROPERTY CXX_STANDARD_REQUIRED 20)
#find_package(fmt)
#target_link_libraries(EquiLand PRIVATE fmt::fmt)

find_package(freetype REQUIRED)
target_link_libraries(EquiLand PRIVATE ${FREETYPE_LIBRARIES})
target_include_directories(EquiLand PRIVATE ${FREETYPE_INCLUDE_DIRS})

find_package(libdivide REQUIRED)
target_link_libraries(EquiLand PRIVATE libdivide::libdivide)

target_link_libraries(EquiLand PRIVATE libpeval)

target_precompile_headers(EquiLand PRIVATE "includes_pch.h")