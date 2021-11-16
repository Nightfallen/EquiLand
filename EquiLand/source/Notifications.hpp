#ifndef SOURCE_NOTIFICATIONS_HPP
#define SOURCE_NOTIFICATIONS_HPP

#include <includes_pch.h>
// A little playground with notifications
// Need to be done in future with icons, custom fonts, animations, blur and blackjack
// Ideas:
// Each notification is a new window and viewport
// But as it's notification it shouldn't be shown in the taskbar (WS_EX_TOOLWINDOW in windows)
// I ran out of ideas for now, i want to sleep :)

int GetCurrentMonitor()
{
	auto cur_window = ImGui::GetCurrentWindow();
	auto monitor = cur_window->Viewport->PlatformMonitor;
	return monitor;
}

ImVec2 GetMonitorResolution(int idMonitor = GetCurrentMonitor())
{
	ImGuiContext& g = *GImGui;
	auto monitor = g.PlatformIO.Monitors[idMonitor];
	return monitor.WorkSize;
}

void Notify(bool* is_open, const char* label, const char* text)
{
	if (!*is_open) return;

	static std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();

	// 5 secs
	if (timeElapsed >= 5000)
	{
		begin = end;
		*is_open = false;
	}

	ImVec2 szNotification = { 200.f, 50.f };
	ImVec2 szNotifyPaddings = { 10.f, 5.f };
	ImVec2 szMonitor = GetMonitorResolution(0);
	auto posWindow = szMonitor - szNotification - szNotifyPaddings;

	auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking;
	ImGui::SetNextWindowPos(posWindow, ImGuiCond_Once);
	ImGui::SetNextWindowSize(szNotification, ImGuiCond_Once);

	auto id = ImGui::GetID(label);
	ImGui::Begin(label, is_open, flags);

	ImGui::Text(label);
	ImGui::Separator();
	ImGui::Text(text);

	ImGui::End();
}

#endif // !SOURCE_NOTIFICATIONS_HPP