#ifndef SOURCE_KEYBOARD_HPP
#define SOURCE_KEYBOARD_HPP

#include <includes_pch.h>

//consteval int KEY_WIN_LINUX_EX(int x = 0, int y = 0)
//{
//#if defined(_WIN_BUILD)
//		return x;
//#elif defined(_LINUX_BUILD)
//		return y;
//#endif
//}

#if defined(_WIN_BUILD)
#define KEY_WIN_LINUX(x, y) \
_Pragma("if defined(_WIN_BUILD)") x \
_Pragma("elif defined(_LINUX_BUILD)")\
_Pragma("endif") 
#elif defined(_LINUX_BUILD)
#define KEY_WIN_LINUX(x, y) \
_Pragma("if defined(_WIN_BUILD)") \
_Pragma("elif defined(_LINUX_BUILD)") y \
_Pragma("endif") 
#endif

namespace VKEYS {
	enum VKEYS {
		ESCAPE		= KEY_WIN_LINUX(VK_ESCAPE, GLFW_KEY_ESCAPE),
		TILDA		= KEY_WIN_LINUX(VK_OEM_3, GLFW_KEY_GRAVE_ACCENT),	// ` OR ~ 
		CAPSLOCK	= KEY_WIN_LINUX(VK_CAPITAL, GLFW_KEY_CAPS_LOCK),
		SHIFT		= KEY_WIN_LINUX(VK_SHIFT, GLFW_KEY_LEFT_SHIFT),
		CTRL		= KEY_WIN_LINUX(VK_CONTROL, GLFW_KEY_LEFT_CONTROL),
		ALT			= KEY_WIN_LINUX(VK_MENU, GLFW_KEY_LEFT_ALT),
		ENTER		= KEY_WIN_LINUX(VK_RETURN, GLFW_KEY_ENTER),
		BACKSPACE	= KEY_WIN_LINUX(VK_BACK, GLFW_KEY_BACKSPACE),
		F1			= KEY_WIN_LINUX(VK_F1, GLFW_KEY_F1),
		F2			= KEY_WIN_LINUX(VK_F2, GLFW_KEY_F2),
		F3			= KEY_WIN_LINUX(VK_F3, GLFW_KEY_F3),
		F4			= KEY_WIN_LINUX(VK_F4, GLFW_KEY_F4),
		F5			= KEY_WIN_LINUX(VK_F5, GLFW_KEY_F5),
		F6			= KEY_WIN_LINUX(VK_F6, GLFW_KEY_F6),
		F7			= KEY_WIN_LINUX(VK_F7, GLFW_KEY_F7),
		F8			= KEY_WIN_LINUX(VK_F8, GLFW_KEY_F8),
		F9			= KEY_WIN_LINUX(VK_F9, GLFW_KEY_F9),
		F10			= KEY_WIN_LINUX(VK_F10, GLFW_KEY_F10),
		F11			= KEY_WIN_LINUX(VK_F11, GLFW_KEY_F11),
		F12			= KEY_WIN_LINUX(VK_F12, GLFW_KEY_F12),
		A			= KEY_WIN_LINUX(0x41, GLFW_KEY_A),
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z			= KEY_WIN_LINUX(0x5A, GLFW_KEY_Z),
		ZERO		= KEY_WIN_LINUX(0x30, GLFW_KEY_0),
		ONE	,
		TWO	,
		THREE,
		FOUR,
		FIVE,
		SIX	,
		SEVEN,
		EIGHT,
		NINE		= KEY_WIN_LINUX(0x39, GLFW_KEY_9),
	};
}

bool IsKeyPressed(int key_code)
{
#if defined(_WIN_BUILD)
	return GetKeyState(key_code) & 0x8000;
#elif defined(_LINUX_BUILD)
	auto& io = ImGui::GetIO();
	return io.KeysDown[key_code];
#endif
}

// It's possible to create any shortcut, even 'CTRL+K+L+Q+W+E'
// But all keys need to be pressed simultaneously
template<typename... Values>
bool ShortCut(Values const&... values)
{
	return (IsKeyPressed(values) && ...);
}

#endif // !SOURCE_KEYBOARD_HPP