#ifndef SOURCE_WIDGETS_BASICCUSTOMWIDGETS_HPP
#define SOURCE_WIDGETS_BASICCUSTOMWIDGETS_HPP

#include <includes_pch.h>
#include <source/utils/ColorHelpers.hpp>

namespace widgets {
	static void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	void DrawSplitter(int split_vertically, float thickness, float* size0, float* size1, float min_size0, float min_size1)
	{
		ImVec2 backup_pos = ImGui::GetCursorPos();
		if (split_vertically)
			ImGui::SetCursorPosY(backup_pos.y + *size0);
		else
			ImGui::SetCursorPosX(backup_pos.x + *size0);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));          // We don't draw while active/pressed because as we move the panes the splitter button will be 1 frame late
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 0.10f));
		ImGui::Button("##Splitter", ImVec2(!split_vertically ? thickness : -1.0f, split_vertically ? thickness : -1.0f));
		ImGui::PopStyleColor(3);

		ImGui::SetItemAllowOverlap(); // This is to allow having other buttons OVER our splitter. 

		if (ImGui::IsItemActive())
		{
			float mouse_delta = split_vertically ? ImGui::GetIO().MouseDelta.y : ImGui::GetIO().MouseDelta.x;

			// Minimum pane size
			if (mouse_delta < min_size0 - *size0)
				mouse_delta = min_size0 - *size0;
			if (mouse_delta > *size1 - min_size1)
				mouse_delta = *size1 - min_size1;

			// Apply resize
			*size0 += mouse_delta;
			*size1 -= mouse_delta;
		}
		ImGui::SetCursorPos(backup_pos);
	}

	bool Splitter(const char* str_id, bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
	{
		using namespace ImGui;
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImGuiID id = window->GetID(str_id);
		ImRect bb;
		bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
		bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
		return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
	}

	bool HyperLinkText(std::string_view label, std::string_view link, bool was_clicked = false)
	{
		ImVec4 color = was_clicked ? ImVec4{ 0.294, 0, 0.509, 0.8 } : ImVec4{ 0, 0, 0.803, 0.8 };
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::Text(label.data());
		ImGui::PopStyleColor();

		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(0))
			{
				system(std::format("start {}", link).data());
				return true;
			}
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			ImGui::SetTooltip("Open link in browser");
		}
		return false;
	}

	void ToggleButton(const char* str_id, bool* v)
	{
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		float height = ImGui::GetFrameHeight();
		float width = height * 2.f;
		float radius = height * 0.50f;

		ImGui::InvisibleButton(str_id, ImVec2(width, height));
		if (ImGui::IsItemClicked())
			*v = !*v;

		float t = *v ? 1.0f : 0.0f;

		ImGuiContext& g = *GImGui;
		float ANIM_SPEED = 0.08f;
		if (g.LastActiveId == g.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
		{
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *v ? (t_anim) : (1.0f - t_anim);
		}

		ImU32 col_bg;
		if (ImGui::IsItemHovered())
			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
		else
			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));

		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
		draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
	}

	void CenteredText(const char* label, const ImVec2& size_arg)
	{
		using namespace ImGui;
		ImGuiWindow* window = GetCurrentWindow();

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImVec2 pos2 = ImVec2((pos.x + size.x), (pos.y + size.y));
		const ImRect bb(pos, pos2);

		ItemSize(size, style.FramePadding.y);

		const ImVec2 pos_min = ImVec2((bb.Min.x + style.FramePadding.x), (bb.Min.y + style.FramePadding.y));
		const ImVec2 pos_max = ImVec2((bb.Max.x - style.FramePadding.x), (bb.Max.y - style.FramePadding.y));

		RenderTextClipped(pos_min, pos_max, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	}

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

	// #TODO add bold fonts to widget in future after creating font management
	//void MessageBox(std::string_view text, int type, void (*callback)(int))
	void AltMessageBox()
	{
		if (ImGui::Button("Modal.."))
			ImGui::OpenPopup("Delete?");

		// Always center this window when appearing
		auto& io = ImGui::GetIO();
		auto DisplaySize = GetMonitorResolution();
		ImVec2 center(DisplaySize.x * 0.5f, DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		const ImVec2 p = ImVec2(24.0f, 24.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, p);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(56.0 / 255.0, 54.0 / 255.0, 50.0 / 255.0, 1.0f));

		if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
		{
			//ImGui::PushFont(font_Bold);

			// Compute button size
			ImGuiContext& g = *GImGui;
			const ImGuiStyle& style = g.Style;
			float margin = (style.WindowPadding.x) * 2.0f;
			float w = ImGui::GetWindowWidth() - margin;
			float h = 35.0f;

			CenteredText("Do you want to save the changes", ImVec2(w, 20));
			CenteredText("made to the document ?", ImVec2(w, 20));
			//ImGui::PopFont();
			//ImGui::PushFont(font);
			CenteredText("\nYou can revert to undo the changes\n \n", ImVec2(w, 40));
			//ImGui::PopFont();

			//ImGui::PushFont(font_Medium);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(199.0 / 255.0, 195.0 / 255.0, 190.0 / 255.0, 1.0f));
			if (ImGui::Button("Save", ImVec2(w, h)))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopStyleColor();

			const ImVec2 spc = ImVec2(7.0f, 16.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spc);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(116.0 / 255.0, 114.0 / 255.0, 109.0 / 255.0, 1.0f));
			if (ImGui::Button("Revert Changes", ImVec2(w, h)))
			{
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Cancel", ImVec2(w, h)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::PopStyleColor();

			ImGui::PopStyleVar(1);
			//ImGui::PopFont();
			ImGui::EndPopup();
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
	}

	void SelectableColor(ImU32 color)
	{
		ImVec2 p_min = ImGui::GetItemRectMin();
		ImVec2 p_max = ImGui::GetItemRectMax();
		ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, color);
	}

	bool CustomSelectable(const char* label, bool* p_selected, ImU32 bg_color, ImGuiSelectableFlags flags, const ImVec2& size_arg)
	{
		auto& style = ImGui::GetStyle();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->ChannelsSplit(2);

		// Channel number is like z-order. Widgets in higher channels are rendered above widgets in lower channels.
		draw_list->ChannelsSetCurrent(1);

		bool result = ImGui::Selectable(label, p_selected, flags, size_arg);
		auto cur_item_flags = ImGui::GetItemFlags();
		bool is_disabled = cur_item_flags & ImGuiItemFlags_Disabled;
		if (is_disabled)
		{
			bg_color = helpers::DarkenColor(bg_color, 15);
		}
		// !ImGui::IsItemActive()
		if (!ImGui::IsItemHovered() && !*p_selected)
		{
			// Render background behind Selectable().
			draw_list->ChannelsSetCurrent(0);
			SelectableColor(bg_color);
		}


		// Commit changes.
		draw_list->ChannelsMerge();
		return result;
	}
}

#endif // !SOURCE_WIDGETS_BASICCUSTOMWIDGETS_HPP