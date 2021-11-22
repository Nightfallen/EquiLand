#ifndef SOURCE_APPLICATIONMENU_HPP
#define SOURCE_APPLICATIONMENU_HPP

#include <includes_pch.h>
#include <source/widgets/BasicCustomWidgets.hpp>
#include <source/widgets/AnimatedCustomWidgets.hpp>
#include <source/Keyboard.hpp>

bool PopupAbout(bool* is_open)
{
	bool result = true;
	ImGui::SetNextWindowSize({ 300.f, 200.f }, ImGuiCond_Appearing);
	if (ImGui::BeginPopupModal("About", is_open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
	{
		ImGui::Text("EquiLand without version\nAnd Under Development :)");
		static bool clicked = false;

		if (widgets::HyperLinkText("Github Page", "https://github.com/Nightfallen/EquiLand", clicked))
			clicked = true;

		// Rewrite to cross-platform
		if (ImGui::IsKeyPressed(VKEYS::ESCAPE))
			*is_open = false;
		ImGui::EndPopup();
	}
	return result;
}

bool PopupSettings(bool* is_open)
{
	bool result = true;
	ImGui::SetNextWindowSize({ 600.f, 650.f }, ImGuiCond_Appearing);
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
		// Rewrite to cross-platform
		if (io.KeysDown[VKEYS::ESCAPE])
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

void CreateMainMenuBar()
{
	static bool popup = false;
	static bool popup_settings = false;
	static void(*ShowPopup)() = []() {};

	// Now Imgui's menu bar doesn't support shortcuts
	// So using temp workaround
	// Soon single (lambda) function can be used for both if clauses (menuitem and shortcut)

	if (ShortCut(VKEYS::CTRL, VKEYS::P))
		popup_settings = true;
	if (ShortCut(VKEYS::CTRL, VKEYS::I))
		;
	if (ShortCut(VKEYS::ALT, VKEYS::F4))
	{
		// #TODO: add handling of this shortcut
	}

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
}


#endif // !SOURCE_APPLICATIONMENU_HPP