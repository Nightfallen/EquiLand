#ifndef SOURCE_GUI_HPP
#define SOURCE_GUI_HPP

#include <includes_pch.h>
#include <source/custom_widgets.hpp>



void CustomWindow(bool* is_open)
{
	static bool only_once = true;
	if (only_once)
	{
		ImGui::SetNextWindowSize({1400.f, 800.f});
		only_once = false;
	}
	ImGui::Begin("EquiLand");
	ImGui::Text("Something coming soon...");

	ImGui::Columns(2, "##mixed11", false);

	widgets::EquiLand::RangeSelect();
	ImGui::NextColumn();

	widgets::EquiLand::BoardSelect();
	ImGui::NextColumn();

	ImGui::Columns(1);

	ImGui::End();
}

void UI_HANDLER(HWND hwnd)
{
	// Our state
	static bool show_demo_window = true;
	static bool show_equiland_window = true;

	if (!show_demo_window)
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	ImGui::ShowDemoWindow(&show_demo_window);
	CustomWindow(&show_equiland_window);
}

#endif // !SOURCE_GUI_HPP 