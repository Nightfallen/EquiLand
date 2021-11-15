#ifndef INCLUDES_PCH_H
#define INCLUDES_PCH_H

// C++ headers
#include <algorithm>
#include <array>
#include <bit>
#include <charconv>
#include <chrono>
#include <compare>
#include <concepts>
#include <ctime>
#include <filesystem>
#include <format>
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
#include <source_location>
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
// Coming soon...

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

	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <d3d11.h>
	#include <dxgi.h>
	#include <tchar.h>
	#include "imgui/imgui_impl_dx11.h"
	#include "imgui/imgui_impl_win32.h"
	#pragma comment(lib, "d3d11.lib")
	#pragma comment(lib, "dxgi.lib")
	#ifdef _WIN64
	#else
	#endif
// #TODO: Support of other OS'es
#elif !defined(_WIN32)
#error "Only Windows platform is supported"
#elif __APPLE__	
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
	 // iOS Simulator
#if TARGET_OS_MACCATALYST
	 // Mac's Catalyst (ports iOS API into Mac, like UIKit).
#elif TARGET_OS_IPHONE
	// iOS device
#elif TARGET_OS_MAC
	// Other kinds of Mac OS
#else
#error "Unknown Apple platform"
#endif
#elif __linux__
#elif __unix__
#elif defined(_POSIX_VERSION)
#else
#error "Unknown compiler"
#endif
#endif

// These macros causing compiling errors in 'OMPEval'
#undef max
#undef min

#endif // !INCLUDES_PCH_H