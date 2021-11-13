#ifndef SOURCE_GUI_HPP
#define SOURCE_GUI_HPP

#include <includes_pch.h>
#include <source/custom_widgets.hpp>



void CustomWindow(bool* is_open)
{
	ImGui::Begin("EquiLand");

	ImGui::Text("Something coming soon...");
	widgets::EquiLand::RangeSelect();
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