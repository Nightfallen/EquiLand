#ifndef SOURCE_APPLICATION_H
#define SOURCE_APPLICATION_H

#include <includes_pch.h>
#include <source/structures.hpp>
#include <source/Freetype.hpp>

#if defined(_WIN_BUILD)
class Application {
	const wchar_t* wndClassName = L"EquiLand App Class";
	const wchar_t* wndName = L"EquiLand";
	FreetypeTest freetypeTest;

	WNDCLASSEXW wc_ = {};
	HWND hwnd_ = {};
	ID3D11Device* g_pd3dDevice = NULL;
	ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	IDXGISwapChain* g_pSwapChain = NULL;
	ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
	ProcUIHandler ui_handler = NULL;

	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK realWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void InitializeWindow();
	void InitializeImgui();
public:
	Application();
	~Application();

	void SetUIHandler(ProcUIHandler handler);
	void Run();

	HWND GetHWND();
};
#elif defined(_LINUX_BUILD) // !_WIN_BUILD

class Application {
	const char* glsl_version = "#version 130";
	const char* wndName = "EquiLand";
	FreetypeTest freetypeTest;
	ProcUIHandler ui_handler = nullptr;
	GLFWwindow* window_ = nullptr;

	void InitializeWindow();
	void InitializeImGui();
public:
	Application();
	~Application();

	void SetUIHandler(ProcUIHandler handler);
	void Run();
};
#endif


#endif // !SOURCE_APPLICATION_H