#ifndef INCLUDES_PCH_H
#define INCLUDES_PCH_H

// C++ headers
#include <algorithm>
#include <array>
#include <bitset>
#include <charconv>
#include <chrono>
#include <compare>
#include <concepts>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <random>
#include <ranges>
#include <regex>
#include <string_view>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

// We aren't using vcpkg's imgui because of (Docking branch)
// imgui headers (Docking branch)
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#ifdef IMGUI_ENABLE_FREETYPE
#include "imgui/imgui_freetype.h"
#endif
// Imgui's backends are included below in the 'OS dependent' section

// Vcpkg headers
#include <ft2build.h>
#include <libdivide.h>
#include <nlohmann/json.hpp>

// Debug options
#if defined(_DEBUG)
#else
#endif

// OS dependent
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#if !defined(UNICODE)
	#define UNICODE
	#define _UNICODE
	#endif
	#define _WIN_BUILD	

	#include <format>
	// I'm not sure if i need this header at all?
	//#include <source_location>
	
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <d3d11.h>
	#include <dxgi.h>
	#include <tchar.h>
	#include <imgui/imgui_impl_dx11.h>
	#include <imgui/imgui_impl_win32.h>
	#pragma comment(lib, "d3d11.lib")
	#pragma comment(lib, "dxgi.lib")
	#ifdef _WIN64
	#else
	#endif
// #TODO: Support of other OS'es, at least Linux and MacOS
#elif defined(linux) || defined(__linux__) || defined(__unix__) || defined(_POSIX_VERSION)
	#define	_LINUX_BUILD
	#define INFINITE            0xFFFFFFFF  // Infinite timeout
	// vcpkg 
	#define FMT_HEADER_ONLY
	#include <fmt/format.h>

	namespace std
	{
		using fmt::format;
	}

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_opengl3_loader.h>
#include <GLFW/glfw3.h>

#elif defined(__APPLE__)
#error "Apple platform currently isn't supported"
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
	 // iOS Simulator
#elif TARGET_OS_MACCATALYST
	 // Mac's Catalyst (ports iOS API into Mac, like UIKit).
#elif TARGET_OS_IPHONE
	// iOS device
#elif TARGET_OS_MAC
	// Other kinds of Mac OS
#else
#error "Unknown Apple platform"
#endif
#endif

// These macros causing compiling errors in 'OMPEval'
#undef max
#undef min

// Project files that aren't changed frequently or at all
#include <fonts/BinaryFontAwesome.hpp>
#include <fonts/BinaryFontRoboto.hpp>
#include <fonts/DefinesFontAwesome.hpp>

#endif // !INCLUDES_PCH_H