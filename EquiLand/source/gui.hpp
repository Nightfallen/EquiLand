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
	ImGui::Columns(3, "##main_ui", false);

	ImGui::SetColumnWidth(0, 650.f);
	ImGui::SetColumnWidth(1, 250.f);
	ImGui::SetColumnWidth(2, 250.f);

	widgets::EquiLand::RangeSelect();
	ImGui::NextColumn();

	widgets::EquiLand::BoardSelect();
	ImGui::NextColumn();

	widgets::EquiLand::DeadCardsSelect();
	ImGui::NextColumn();
	ImGui::Columns(1);

	ImGui::End();
}

void SelectableColor(ImVec2 szSelectable, ImU32 color)
{
	auto& style = ImGui::GetStyle();
	ImVec2 itemSpacing = style.ItemSpacing;
	ImVec2 p_min = ImGui::GetCursorScreenPos() - style.FramePadding;
	ImVec2 p_max = p_min + itemSpacing + szSelectable; 
	ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, color);
}

void SelectableColorEx(ImVec2 prevCurPos, ImVec2 szSelectable, ImU32 color)
{
	auto& style = ImGui::GetStyle();
	ImVec2 itemSpacing = style.ItemSpacing;
	ImVec2 p_min = prevCurPos - style.FramePadding;
	ImVec2 p_max = p_min + itemSpacing + szSelectable;
	ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, color);
}

void MyImguiWindow(bool* is_open)
{
	ImGui::Begin("##Temp Window", is_open);
	constexpr int szArr = 9;
	static bool selectables[szArr] = {};
	ImVec2 szEl = ImVec2(40, 40);
	auto flags = ImGuiSelectableFlags_SelectOnClick;

	// Text of selectable is behind rect
	bool separator = false;
	for (int i = 0; i < 9; ++i)
	{
		if (separator) ImGui::SameLine();
		std::string label = std::to_string(i + 1);
		auto prevCurPos = ImGui::GetCursorScreenPos();
		ImGui::Selectable(label.data(), &selectables[i], flags, szEl);

		if (!ImGui::IsItemHovered() && !selectables[i])
		{
			SelectableColorEx(prevCurPos, szEl, IM_COL32(255, 0, 0, 200));
		}
		separator = true;
	}

	// Text of selectable is above rect, but i can't use 'ImGui::IsItemHovered()'
	separator = false;
	for (int i = 0; i < 9; ++i)
	{
		if (separator) ImGui::SameLine();
		std::string label = std::to_string(i + 1);
		auto prevCurPos = ImGui::GetCursorScreenPos();

		if (!ImGui::IsItemHovered() && !selectables[i])
		{
			SelectableColor(szEl, IM_COL32(255, 0, 0, 200));
		}
		ImGui::Selectable(label.data(), &selectables[i], flags, szEl);
		separator = true;
	}
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
	CustomWindow(&show_equiland_window);
	MyImguiWindow(&show_temp_window);
}

#endif // !SOURCE_GUI_HPP 