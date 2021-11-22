#ifndef SOURCE_GUI_HPP
#define SOURCE_GUI_HPP

#include <includes_pch.h>
#include <source/widgets/IncludeAllWidgets.hpp>
#include <fonts/DefinesFontAwesome.hpp>
#include <source/Notifications.hpp>
#include <source/ApplicationMenu.hpp>
#include <source/AppSettings.hpp>
#include <source/Keyboard.hpp>

void CustomWindow(bool* is_open)
{
	auto& style = ImGui::GetStyle();
	auto& io = ImGui::GetIO();
	ImGui::SetNextWindowSize({ 1400.f, 800.f }, ImGuiCond_Once);
	auto windows_flags = ImGuiWindowFlags_MenuBar;
	ImGui::Begin("EquiLand", is_open, windows_flags);
	CreateMainMenuBar();

	static widgets::EquiLand::CardMatrix range_matrix = {};
	static widgets::EquiLand::Cards dead_cards = {};
	static widgets::EquiLand::Cards board_cards = {};
	auto& range = range_matrix.range;
	auto posCursor = ImGui::GetCursorStartPos();
	auto wndSize = ImGui::GetWindowSize();
	
	auto paddings = style.FramePadding + style.WindowPadding;
	auto result = wndSize - posCursor - paddings;
	float h = result.y; //800 - paddings.y;
	float w = result.x; // 1400 - paddings.x;
	static float spl1 = 250;
	static float spl2 = 650;
	static float spl3 =  250;
	static float spl4 =  250;

	float child_w1 = spl1;
	float child_w2 = spl2;
	float child_w3 = spl3;
	float child_w4 = spl4;

	widgets::Splitter("##Splitter1", true, 8.0f, &spl1, &spl2, 8, 8, h);
	static std::bitset<169>* range_ptr = nullptr;
	std::bitset<169>* temp_range_ptr;
	bool changed_range_node = false;
	if (ImGui::BeginChild("child1", ImVec2(child_w1, h), true))
	{
		temp_range_ptr = widgets::EquiLand::TreeRangeEditor();
		if (temp_range_ptr != nullptr)
		{
			if (temp_range_ptr != range_ptr)
			{
				range_ptr = temp_range_ptr;
				changed_range_node = true;
			}
		}
	}
	ImGui::EndChild();

	ImGui::SameLine();
	widgets::Splitter("##Splitter2", true, 8.f, &spl2, &spl3, 8, 8, h);
	ImGui::SameLine();
	if (ImGui::BeginChild("child2", ImVec2(child_w2, h), true))
	{
		//widgets::EquiLand::RangeSelect(range_matrix);
		if (range_ptr != nullptr)
			range_matrix.cards = *range_ptr;

		widgets::EquiLand::RangeSelect(range_matrix, changed_range_node);

		if (range_ptr != nullptr)
			for (int i = 0; i < 169; ++i)
				range_ptr->set(i, range_matrix.cards[i]);
	}
	ImGui::EndChild();

	ImGui::SameLine();
	widgets::Splitter("##Splitter3", true, 8.f, &spl3, &spl4, 8, 8, h);
	ImGui::SameLine();
	if (ImGui::BeginChild("child3", ImVec2(child_w3, h), true))
	{	
		widgets::EquiLand::BoardSelect<5>(board_cards, true, dead_cards);
	}
	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginChild("child4", ImVec2(child_w4, h), true);
	widgets::EquiLand::DeadCardsSelect<2>(dead_cards, range, true, board_cards);
	ImGui::EndChild();
	
	ImGui::End();
}

// Playground window
void PlaygroundWindow(bool* is_open)
{
	auto& io = ImGui::GetIO();
	auto& style = ImGui::GetStyle();
	ImGui::SetNextWindowSize(ImVec2(1000, 600), ImGuiCond_Once);
	ImGui::Begin("##window_no_name1", is_open);

	ImGui::End();
}

//void UI_HANDLER(HWND hwnd)
void UI_HANDLER()
{
	// Our state
	static NotificationSystem notifySystem;


	static bool just_once = true;
	if (just_once)
	{
		auto custom_content = []() {
			ImGui::Text("Did you open it? Or at least hover?");
			widgets::HyperLinkText("Open GitHub page", "https://github.com/Nightfallen/EquiLand");
		};

		notifySystem.AddNotificationIcon("Export history ready", "Check your '{PATH}' or click open", ICON_FA_CHECK, ImVec4(0, 1, 0, 1));
		notifySystem.AddNotificationIcon("Update Error", "You need to reinstall app", ICON_FA_BAN, ImVec4(1, 0, 0, 1));
		notifySystem.AddNotificationCustomElements("Awesome notify", ICON_FA_INFO, ImVec4(0.117, 0.564, 1, 1.f), custom_content);

		just_once = false;
	}

	static bool show_demo_window = true;
	static bool show_equiland_window = true;
	static bool show_temp_window = false;

	//if (!show_demo_window)
	//	PostMessage(hwnd, WM_CLOSE, 0, 0);
	//ImGui::ShowDemoWindow(&show_demo_window);
	//if (!show_equiland_window)
	//	PostMessage(hwnd, WM_CLOSE, 0, 0);
	if (!show_demo_window)
		exit(0);
	ImGui::ShowDemoWindow(&show_demo_window);


	static bool first_time = true;
	if (first_time)
	{
		ImGui::GetStyle().Alpha = 0.f;
		first_time = false;
	}

	{
		constexpr auto frequency = 1.2f;
		auto deltaTime = ImGui::GetIO().DeltaTime;
		auto& alpha = ImGui::GetStyle().Alpha;
		alpha = ImClamp(alpha + frequency * deltaTime, 0.f, 1.f);
	}

	CustomWindow(&show_equiland_window);
	if (!show_equiland_window)
	{
		constexpr auto frequency = 1.f / 0.5f;
		auto deltaTime = ImGui::GetIO().DeltaTime;
		auto& alpha = ImGui::GetStyle().Alpha;
		auto sub_alpha = alpha - frequency * deltaTime;
		std::cout << std::format("Sub alpha: {}\n", sub_alpha);
		alpha = ImClamp(sub_alpha, 0.f, 1.f);
		if (alpha == 0)
			exit(0);
	}

	if (show_temp_window)
		PlaygroundWindow(&show_temp_window);
	notifySystem.RenderNotifications();
}

#endif // !SOURCE_GUI_HPP 