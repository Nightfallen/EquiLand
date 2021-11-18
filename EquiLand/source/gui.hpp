#ifndef SOURCE_GUI_HPP
#define SOURCE_GUI_HPP

#include <includes_pch.h>
#include <source/custom_widgets.hpp>
#include <fonts/DefinesFontAwesome.hpp>
#include <source/Notifications.hpp>

bool PopupAbout(bool* is_open)
{
	bool result = true;
	ImGui::SetNextWindowSize({ 300.f, 200.f });
	if (ImGui::BeginPopupModal("About", is_open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
	{
		ImGui::Text("EquiLand without version\nAnd Under Development :)");
		static bool clicked = false;

		if (widgets::HyperLinkText("Github Page", "https://github.com/Nightfallen/EquiLand", clicked))
			clicked = true;

		if (ImGui::IsKeyPressed(VK_ESCAPE))
			*is_open = false;
		ImGui::EndPopup();
	}
	return result;
}

bool PopupSettings(bool* is_open)
{
	bool result = true;
	ImGui::SetNextWindowSize({ 600.f, 650.f });
	if (ImGui::BeginPopupModal("Settings", is_open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
	{
		auto& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();
		ImGui::Text("EquiLand settings:");

		ImGui::Text("Equity Calculation Settings:");
		static int algEquity = 0;
		ImGui::RadioButton("Enumeration", &algEquity, 0); ImGui::SameLine();
		ImGui::RadioButton("Monte Carlo", &algEquity, 1);

		bool isMonteCarlo = algEquity != 1;
		ImGui::BeginDisabled(isMonteCarlo);
		static float timeout = 0;
		ImGui::DragFloat("Evaluation timeout", &timeout, 0.05f, 0, 60, "%.2fs");
		static int max_evaluations = 10'000'000;
		ImGui::DragInt("Max evaluations", &max_evaluations, 100'000.f, 0, 1'000'000'000);
		static float tolerance = 2e-5;
		ImGui::DragFloat("Tolerance", &tolerance, 2e-5, -10.f, 10.f, "%e");
		ImGui::EndDisabled();

		// Future user-styling preferences
		ImGui::Text("Styling preferences");
		static float col1[3] = { 1.0f, 0.0f, 0.2f };
		static float col2[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
		ImGui::ColorEdit3("Selected range", col1);
		ImGui::ColorEdit4("Hovered range", col2);

		ImGui::Text("Expert settings");
		static bool opt_val1 = false;
		static bool opt_val2 = false;
		static bool opt_val3 = false;
		const char* opt1 = "Save research history";
		const char* opt2 = "Integrate with some other program/service";
		const char* opt3 = "Take sreenshot of research (Beta)";
		constexpr size_t szBuf = 256;
		static char buf[szBuf] = {};

		widgets::ToggleButton(opt1, &opt_val1);
		ImGui::SameLine();
		ImGui::Text(opt1);
		ImGui::BeginDisabled(!opt_val1);
		ImGui::InputText("Folder to save", buf, szBuf);
		ImGui::EndDisabled();
		widgets::ToggleButton(opt2, &opt_val2);
		ImGui::SameLine();
		ImGui::Text(opt2);
		widgets::ToggleButton(opt3, &opt_val3);
		ImGui::SameLine();
		ImGui::Text(opt3);

		auto data_type = ImGuiDataType_S32;
		int min = 0;
		int max = 100;
		static int cur_val = 0;
		static bool opt_val4 = false;
		widgets::public_untested::AnimatedSlider("Some option", data_type, &cur_val, &min, &max, " %d%% ", 0);
		widgets::public_untested::AnimatedCheckbox("Some Option x2", &opt_val4);
		widgets::public_untested::AnimatedButton("Cool button to apply settings");

		//if (ImGui::Button("msg box"))
		widgets::AltMessageBox();
		
		if (io.KeysDown[VK_ESCAPE])
		{
			*is_open = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	return result;
}

bool PopupUpdate(bool* is_open)
{
	bool result = true;
	return result;
}

std::string MenuItemText(std::string_view text, std::string_view icon = ICON_FA_AMAZON)
{
	auto szIcon = ImGui::CalcTextSize(icon.data());
	auto szSpace = ImGui::CalcTextSize(" ");
	int spaces = 0;

	while (szSpace.x < szIcon.x * 2)
	{
		++spaces;
		szSpace.x += szSpace.x;
	}

	std::string result;
	result.append(icon);
	while (spaces--)
		result.append(" ");
	result.append(text);
	return result;
}

void CustomWindow(bool* is_open)
{
	auto& style = ImGui::GetStyle();
	auto& io = ImGui::GetIO();
	ImGui::SetNextWindowSize({ 1400.f, 800.f }, ImGuiCond_Once);
	auto windows_flags = ImGuiWindowFlags_MenuBar;
	ImGui::Begin("EquiLand", is_open, windows_flags);
	static bool popup = false;
	static bool popup_settings = false;
	static void(*ShowPopup)() = []() {};
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File##my_file"))
		{
			if (ImGui::MenuItemEx("Import Ranges", ICON_FA_FILE, "CTRL+I")) {}
			ImGui::Separator();
			if (ImGui::MenuItemEx("Exit##exit", ICON_FA_TIMES_CIRCLE, "ALT+F4")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools##my_tools"))
		{
			if (ImGui::MenuItem("Quiz")) {}
			ImGui::Separator();
			if (ImGui::MenuItemEx("Options", ICON_FA_COG, "CTRL+P"))
			{
				popup_settings = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItemEx("About", ICON_FA_INFO))
			{
				popup = true;
			}
			ImGui::EndMenu();
		}

		bool update_available = true;
		if (update_available)
		{
			if (ImGui::MenuItem("Update Available"))
			{

			}
		}

		ImGui::EndMenuBar();
	}

	if (!ImGui::IsPopupOpen("About") && popup)
		ImGui::OpenPopup("About");
	PopupAbout(&popup);

	if (!ImGui::IsPopupOpen("Settings") && popup_settings)
		ImGui::OpenPopup("Settings");
	PopupSettings(&popup_settings);

	/*
	 Maybe i can use this later
	 Lambda approach to popups from menu:

	 static void(*ShowPopup)() = []() {};
	 ...
	 if (ImGui::MenuItem("some_item") 
	 {
		ShowPopup = []()
		{
			if (!ImGui::IsPopupOpen("popup"))
				ImGui::OpenPopup("popup");

			if (ImGui::BeginPopupModal("popup"))
			{
				ImGui::Text("Lorem ipsum");

				if (ImGui::Button("Close", ImVec2(80, 0)))
				{
					ImGui::CloseCurrentPopup();
					ShowPopup = []() {};
				}
				ImGui::EndPopup();
			}
		};
	 }
	 ...
	 ShowPopup();
	*/

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

	if (ImGui::BeginChild("child1", ImVec2(child_w1, h), true))
	{
		widgets::EquiLand::TreeRangeEditor();
		ImGui::EndChild();
	}
	ImGui::SameLine();
	widgets::Splitter("##Splitter2", true, 8.f, &spl2, &spl3, 8, 8, h);
	ImGui::SameLine();
	if (ImGui::BeginChild("child2", ImVec2(child_w2, h), true))
	{
		widgets::EquiLand::RangeSelect(range_matrix);
		ImGui::EndChild();
	}
	ImGui::SameLine();
	widgets::Splitter("##Splitter3", true, 8.f, &spl3, &spl4, 8, 8, h);
	ImGui::SameLine();
	if (ImGui::BeginChild("child3", ImVec2(child_w3, h), true))
	{	
		widgets::EquiLand::BoardSelect<5>(board_cards, true, dead_cards);
		ImGui::EndChild();
	}

	ImGui::SameLine();
	//if (ImGui::BeginChild("child4", ImVec2(sz4, h), true))
	{	
		ImGui::BeginChild("child4", ImVec2(child_w4, h), true);
		widgets::EquiLand::DeadCardsSelect<2>(dead_cards, range, true, board_cards);
		ImGui::EndChild();
	}



	ImGui::End();
}

// Playground window
void PlaygroundWindow(bool* is_open)
{
	ImGui::SetNextWindowSize(ImVec2(1000, 600), ImGuiCond_Once);
	ImGui::Begin("##window_no_name1", is_open);

	ImGui::End();
}

void UI_HANDLER(HWND hwnd)
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

	if (!show_demo_window)
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	ImGui::ShowDemoWindow(&show_demo_window);
	if (!show_equiland_window)
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	CustomWindow(&show_equiland_window);
	if (show_temp_window)
		PlaygroundWindow(&show_temp_window);

	notifySystem.RenderNotifications();
}

#endif // !SOURCE_GUI_HPP 