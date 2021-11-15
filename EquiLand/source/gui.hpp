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
	ImGui::Begin("EquiLand",is_open);
	ImGui::Text("Something coming soon...");
	ImGui::Columns(3, "##main_ui", false);

	ImGui::SetColumnWidth(0, 650.f);
	ImGui::SetColumnWidth(1, 250.f);
	ImGui::SetColumnWidth(2, 250.f);

	static widgets::EquiLand::CardMatrix range_matrix = {};
	widgets::EquiLand::RangeSelect(range_matrix);
	ImGui::NextColumn();


	static widgets::EquiLand::Cards dead_cards = {};
	static widgets::EquiLand::Cards board_cards = {};
	widgets::EquiLand::BoardSelect<5>(board_cards, true, dead_cards);
	ImGui::NextColumn();

	//widgets::EquiLand::DeadCardsSelect();
	auto& range = range_matrix.range;
	widgets::EquiLand::DeadCardsSelect<2>(dead_cards, range, true, board_cards);
	ImGui::NextColumn();
	ImGui::Columns(1);

	ImGui::End();
}

void UI_HANDLER(HWND hwnd)
{
	// Our state
	static bool show_demo_window = true;
	static bool show_equiland_window = true;
	static bool show_temp_window = true;

	if (!show_demo_window)
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	ImGui::ShowDemoWindow(&show_demo_window);
	if (!show_equiland_window)
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	CustomWindow(&show_equiland_window);
}

#endif // !SOURCE_GUI_HPP 