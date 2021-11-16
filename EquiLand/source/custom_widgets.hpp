#ifndef SOURCE_CUSTOM_WIDGETS_HPP
#define SOURCE_CUSTOM_WIDGETS_HPP

#include <includes_pch.h>

#include <fonts/DefinesFontAwesome.hpp>
#include "OMPEval/HandEvaluator.h"
#include "OMPEval/EquityCalculator.h"

// Untested code of imgui widgets i found searching the net
// Need to rework them or discard if they won't be needed in the future
namespace widgets::public_untested {

	// I fount it on the net, very good idea
	// But i need to reimplement it to understand how it works 
	// And also redesign it to not using 'Imgui::Begin()' and 'ImGui::End()' functions,
	// cause it's very confusing, there are 
	// 
	bool AnimatedSlider(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
	{
		// This widget doesn't calculate frame_bb with the 'label' impact
		// Temp workaround
		ImGui::InvisibleButton(std::format("##11{}", label).data(), ImGui::CalcTextSize(label));
		using namespace ImGui;

		static std::map<ImGuiID, float> padding_anim;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);

		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
		const ImRect total_bb(frame_bb.Min - ImVec2(0, label_size.y + style.ItemInnerSpacing.y), frame_bb.Max);

		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, id, &frame_bb))
			return false;

		// Tabbing or CTRL-clicking on Slider turns it into an input box
		const bool hovered = ItemHoverable(frame_bb, id);
		const bool hovered_plus = ItemHoverable(total_bb, id);

		const bool temp_input_allowed = false;
		bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
		if (!temp_input_is_active)
		{

			const bool focus_requested = temp_input_allowed && FocusableItemRegister(window, id);
			const bool clicked = (hovered && g.IO.MouseClicked[0]);
			//if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
			if (focus_requested || clicked || g.NavActivateId == id || g.ActiveIdUsingNavInputMask == id)
			{
				SetActiveID(id, window);
				SetFocusID(id, window);
				FocusWindow(window);
				g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
				//if (temp_input_allowed && (focus_requested || (clicked && g.IO.KeyCtrl) || g.NavInputId == id))
				if (temp_input_allowed && (focus_requested || (clicked && g.IO.KeyCtrl) || g.ActiveIdUsingNavInputMask == id))
				{
					temp_input_is_active = true;
					FocusableItemUnregister(window);
				}
			}
		}

		if (temp_input_is_active)
		{
			// Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
			const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
			return TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
		}

		// Draw frame
		const ImU32 frame_col = GetColorU32(ImGuiCol_FrameBg);
		RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

		// Slider behavior
		ImRect grab_bb;
		const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
		if (value_changed)
			MarkItemEdited(id);

		auto color_frame = style.Colors[ImGuiCol_FrameBg];
		if (grab_bb.Max.x > grab_bb.Min.x)
			RenderFrame(frame_bb.Min, ImVec2(grab_bb.Max.x, frame_bb.Max.y), GetColorU32(color_frame), true, g.Style.FrameRounding);

		// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
		char value_buf[64];
		const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

		// label
		if (label_size.x > 0)
			RenderText(ImVec2(frame_bb.Min.x, frame_bb.Min.y - style.ItemInnerSpacing.y - label_size.y), label);

		// value size
		const ImVec2 value_size = CalcTextSize(value_buf, value_buf_end, true);

		auto it_padding = padding_anim.find(id);
		if (it_padding == padding_anim.end())
		{
			padding_anim.insert({ id, {0.f} });
			it_padding = padding_anim.find(id);
		}

		it_padding->second = ImClamp(it_padding->second + (2.5f * GetIO().DeltaTime * (hovered_plus || GetActiveID() == id ? 1.f : -1.f)), 0.f, 1.f);

		// value
		if (value_size.x > 0.0f && it_padding->second > 0.f)
		{
			auto value_col = GetColorU32(ImGuiCol_FrameBg, it_padding->second);
			PushStyleVar(ImGuiStyleVar_Alpha, it_padding->second);

			char window_name[16];
			ImFormatString(window_name, IM_ARRAYSIZE(window_name), "##tp_%s", label);

			SetNextWindowPos(frame_bb.Max - ImVec2(frame_bb.Max.x - frame_bb.Min.x, 0) / 2 - ImVec2(value_size.x / 2 + 3.f, 0.f));
			SetNextWindowSize((frame_bb.Max - ImVec2(frame_bb.Max.x - frame_bb.Min.x, 0) / 2 + ImVec2(value_size.x / 2 + 3.f, value_size.y + 6)) - (frame_bb.Max - ImVec2(frame_bb.Max.x - frame_bb.Min.x, 0) / 2 - ImVec2(value_size.x / 2 + 3.f, 0.f)));
			SetNextWindowBgAlpha(it_padding->second);

			ImGuiWindowFlags flags = ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration;

			Begin(window_name, NULL, flags);

			RenderFrame(frame_bb.Min + ImVec2(0.f, (frame_bb.Max.y - frame_bb.Min.y)), frame_bb.Min + ImVec2((frame_bb.Max.x - frame_bb.Min.x), (frame_bb.Max.y - frame_bb.Min.y) + value_size.y + 6), GetColorU32(ImGuiCol_FrameBg), GetStyle().WindowRounding);
			RenderTextClipped(frame_bb.Min + ImVec2(0.f, (frame_bb.Max.y - frame_bb.Min.y)), frame_bb.Min + ImVec2((frame_bb.Max.x - frame_bb.Min.x), (frame_bb.Max.y - frame_bb.Min.y) + value_size.y + 6), value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

			End();

			PopStyleVar();
		}
		return value_changed;
	}

	struct animation {
		bool clicked;
		bool reverse;
		float size;
		float mult;
	};

	bool AnimatedCheckbox(const char* label, bool* value)
	{
		using namespace ImGui;
		auto name = label;
		auto callback = value;
		// callback == v
		// name == label
		static std::map<ImGuiID, animation> circle_anim;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(name);
		const ImVec2 label_size = CalcTextSize(name, NULL, true);

		const float square_sz = GetFrameHeight();
		const ImVec2 pos = window->DC.CursorPos;
		const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
		if (pressed)
		{
			*callback = !(*callback);
			MarkItemEdited(id);
		}

		const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));

		RenderFrame(check_bb.Min, check_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
		ImU32 check_col = GetColorU32(ImGuiCol_CheckMark);

		float radius = square_sz * 0.82f;
		auto it_circle = circle_anim.find(id);
		if (it_circle == circle_anim.end())
		{
			circle_anim.insert({ id, {false, false, 0.f, 0.f} });
			it_circle = circle_anim.find(id);
		}

		if (pressed && *callback)
			it_circle->second.clicked = true;
		else if (pressed && !(*callback) && it_circle->second.clicked)
		{
			it_circle->second.mult = 0.f;
			it_circle->second.size = 0.f;

			it_circle->second.reverse = false;
			it_circle->second.clicked = false;
		}

		if (it_circle->second.clicked)
		{
			if (!it_circle->second.reverse)
			{
				it_circle->second.size = ImClamp(it_circle->second.size + 3.f * GetIO().DeltaTime, 0.f, 1.f);
				it_circle->second.mult = ImClamp(it_circle->second.mult + 6.f * GetIO().DeltaTime, 0.f, 1.f);

				if (it_circle->second.mult >= 0.99f)
					it_circle->second.reverse = true;
			}
			else
			{
				it_circle->second.size = ImClamp(it_circle->second.size + 3.f * GetIO().DeltaTime, 0.f, 1.f);
				it_circle->second.mult = ImClamp(it_circle->second.mult - 6.f * GetIO().DeltaTime, 0.f, 1.f);

				if (it_circle->second.mult <= 0.01f)
				{
					it_circle->second.mult = 0.f;
					it_circle->second.size = 0.f;

					it_circle->second.reverse = false;
					it_circle->second.clicked = false;
				}

			}
		}


		if (*callback)
		{
			auto color = style.Colors[ImGuiCol_FrameBg];
			RenderFrame(check_bb.Min, check_bb.Max, GetColorU32(color), true, style.FrameRounding);

			const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 5.f));
			RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), check_col, square_sz - pad * 2.0f);
		}

		ImU32 circle_col = GetColorU32(ImGuiCol_CheckMark, it_circle->second.mult * 0.7f);

		if (it_circle->second.mult > 0.01f)
			window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2, check_bb.Min.y + (check_bb.Max.y - check_bb.Min.y) / 2), radius * it_circle->second.size, circle_col, 30);

		// Better use clipped text (aligns and some else..)
		if (label_size.x > 0.0f)
			RenderText(ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + ((check_bb.Max.y - check_bb.Min.y) / 2) - label_size.y / 2 - 1), name);

		return pressed;
	}

	struct button_animation {
		bool clicked;
		bool reverse;
		float size;
		float mult;
		ImVec2 pos;
	};

	bool AnimatedButton(const char* label, ImVec2 size_arg = ImVec2(0, 0))
	{
		using namespace ImGui;
		auto name = label;
		auto a_size = size_arg;
		// name == label

		static std::map<ImGuiID, button_animation> button_circle;
		static std::map<ImGuiID, float> button_hover;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(name);
		const ImVec2 label_size = CalcTextSize(name, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = CalcItemSize(a_size, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ItemSize(size, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held, 0);

		// Render
		const ImU32 col = GetColorU32(ImGuiCol_Button);
		RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

		// Press button animation (Drawing circle in clipped area)
		auto it_circle = button_circle.find(id);
		if (it_circle == button_circle.end())
		{
			button_circle.insert({ id, {false, false, 0.f, 0.f, ImVec2(0, 0)} });
			it_circle = button_circle.find(id);
		}

		// If pressed and wasn't active - start animation
		if (pressed && !it_circle->second.clicked)
		{
			it_circle->second.clicked = true;
			it_circle->second.pos = GetIO().MousePos;
		}

		// If pressed and was active - restart animation
		else if (pressed && it_circle->second.clicked)
		{
			it_circle->second.mult = 0.f;
			it_circle->second.size = 0.f;

			it_circle->second.reverse = false;
			it_circle->second.clicked = true;

			it_circle->second.pos = GetIO().MousePos;
		}

		// If we have active animation - procedure it
		if (it_circle->second.clicked)
		{
			// First stage - size(+) alpha(-)
			if (!it_circle->second.reverse)
			{
				it_circle->second.size = ImClamp(it_circle->second.size + 1.5f * GetIO().DeltaTime, 0.f, 1.f);
				it_circle->second.mult = ImClamp(it_circle->second.mult + 2.f * GetIO().DeltaTime, 0.f, 1.f);

				// Go for the second stage
				if (it_circle->second.mult >= 0.99f)
					it_circle->second.reverse = true;
			}

			// Second stage - size(+) alpha(-)
			else
			{
				it_circle->second.size = ImClamp(it_circle->second.size + 1.5f * GetIO().DeltaTime, 0.f, 1.f);
				it_circle->second.mult = ImClamp(it_circle->second.mult - 2.f * GetIO().DeltaTime, 0.f, 1.f);

				// Stop animation
				if (it_circle->second.mult <= 0.01f)
				{
					it_circle->second.mult = 0.f;
					it_circle->second.size = 0.f;

					it_circle->second.reverse = false;
					it_circle->second.clicked = false;
				}

			}
		}

		// Animated circle (on-press animation) color
		ImVec4 circle_color = ImVec4(0.98f, 0.98f, 0.98f, it_circle->second.mult * 0.6f);

		// Circle radius calcalution
		float radius = ImMax(fabs(it_circle->second.pos.x - bb.Max.x), fabs(it_circle->second.pos.x - bb.Min.x)) * 2.f;

		// Clip area to draw a cicrle inside frame
		window->DrawList->PushClipRect(bb.Min, bb.Max);
		window->DrawList->AddCircleFilled(it_circle->second.pos, radius * it_circle->second.size, GetColorU32(circle_color), 30);
		window->DrawList->PopClipRect();

		// hover animation
		auto it_hover = button_hover.find(id);
		if (it_hover == button_hover.end())
		{
			button_hover.insert({ id, {0.f} });
			it_hover = button_hover.find(id);
		}

		it_hover->second = ImClamp(it_hover->second + 2.5f * GetIO().DeltaTime * (hovered ? 1.f : -1.f), 0.f, 1.f);

		ImVec4 hovering_color = ImVec4(0.98f, 0.98f, 0.98f, it_hover->second * 0.4f);

		window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(hovering_color), GetStyle().FrameRounding);

		ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, name, NULL, &label_size, style.ButtonTextAlign, &bb);

		return pressed;


	}

}

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
			auto red = bg_color >> 24;
			auto green = (bg_color << 8) >> 8 + 16;
			auto blue = (bg_color << 16) >> 16 + 8;
			auto alpha = (bg_color << 24) >> 24;
			int correction_factor = 10;
			red = (255 - red) * correction_factor + red;
			green = (255 - green) * correction_factor + green;
			blue = (255 - blue) * correction_factor + blue;

			bg_color = IM_COL32(red, green, blue, alpha);

		}
		if (!ImGui::IsItemHovered() && !ImGui::IsItemActive() && !*p_selected)
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

namespace widgets::EquiLand {

	// Helper function for 'RangeSelect' widget
	// #TODO: make another algorithm
	template<typename T, std::size_t N>
	std::string BuildRangeString(const T(&arr)[N])
	{
		std::string result;
		result.reserve(256);
		static 	const std::unordered_map<int, char> rankMap = {
		{0, 'A'}, {1, 'K'}, {2, 'Q'}, {3, 'J'}, {4, 'T'}, {5, '9'},
		{6, '8'}, {7, '7'},   {8, '6'}, {9, '5'}, {10, '4'},  {11, '3'}, {12, '2'}
		};

		bool isRandom = std::all_of(arr, arr + N, [](bool x) { return x == true; });
		if (isRandom)
			return "random";

		// Constructing pairs
		const char* separator = "";
		int nMax = std::sqrt(N);

		// Some unreachable value
		int prev_pp = 0xff;
		int prev_os = 0xff;
		int prev_ss = 0xff;

		const int max_str_ranges = 7;
		std::string pair_res;
		//std::string last_pairs[max_pairs_ranges] = {};
		std::vector<std::string> last_pairs = {};
		last_pairs.reserve(max_str_ranges);

		// off-suited results
		std::vector<std::string> os_res = {};
		os_res.reserve(max_str_ranges * 12);

		// single-suited results
		std::vector<std::string> ss_res = {};
		ss_res.reserve(max_str_ranges * 12);

		for (int x = 0; x < nMax; ++x)
		{
			std::string suit = "o";
			// columns
			for (int y = 0; y < nMax; ++y)
			{
				auto selected = arr[13 * y + x];
				if (selected && x < y)
				{
					int cur_os = 13 * x + y;
					if (cur_os != prev_os)
					{
						if (os_res.empty())
							os_res.push_back(std::format("{}{}{}", rankMap.at(x), rankMap.at(y), suit));
						else if (cur_os == prev_os + 1)
						{
							auto cur_index = os_res.size() - 1;

							auto nPlusIndex = os_res[cur_index].find("+");
							auto nDashIndex = os_res[cur_index].find("-");
							bool isPlusFound = nPlusIndex != std::string::npos;
							bool isDashFound = nDashIndex != std::string::npos;
							bool isFound = isPlusFound || isDashFound;

							if (isPlusFound)
							{
								auto& str = os_res[cur_index];
								str = std::format("{}{}{}+", rankMap.at(x), rankMap.at(y), suit);
							}

							if (isDashFound)
							{
								auto& str = os_res[cur_index];
								str.erase(str.begin() + nDashIndex, str.end());
								str.append(std::format("-{}{}{}", rankMap.at(x), rankMap.at(y), suit));
							}

							if (isFound) {
								prev_os = cur_os;
								continue;
							}

							//if (ss_res[cur_index].find())
							bool is_pair_before_prev = x == y - 1 - 1;
							bool is_pair_left = (x - 1) == y;
							if (is_pair_before_prev)
							{
								os_res[cur_index] = std::format("{}{}{}+", rankMap.at(x), rankMap.at(y), suit);
							}
							else
							{
								os_res[cur_index].append(std::format("-{}{}{}", rankMap.at(x), rankMap.at(y), suit));
							}
						}
						else
						{
							auto temp_str = std::format("{}{}{}", rankMap.at(x), rankMap.at(y), suit);
							os_res.push_back(temp_str);
						}
					}
					prev_os = cur_os;
				}
			}
		}

		for (int y = 0; y < nMax; ++y)
		{
			std::string suit = "s";

			for (int x = 0; x < nMax; ++x)
			{
				auto selected = arr[13 * y + x];
				if (selected && x > y)
				{
					int cur_ss = 13 * y + x;
					if (cur_ss != prev_ss)
					{
						if (ss_res.empty())
							ss_res.push_back(std::format("{}{}{}", rankMap.at(y), rankMap.at(x), suit));
						else if (cur_ss == prev_ss + 1)
						{
							auto cur_index = ss_res.size() - 1;

							auto nPlusIndex = ss_res[cur_index].find("+");
							auto nDashIndex = ss_res[cur_index].find("-");
							bool isPlusFound = nPlusIndex != std::string::npos;
							bool isDashFound = nDashIndex != std::string::npos;
							bool isFound = isPlusFound || isDashFound;

							if (isPlusFound)
							{
								auto& str = ss_res[cur_index];
								str = std::format("{}{}{}+", rankMap.at(y), rankMap.at(x), suit);
							}

							if (isDashFound)
							{
								auto& str = ss_res[cur_index];
								str.erase(str.begin() + nDashIndex, str.end());
								str.append(std::format("-{}{}{}", rankMap.at(y), rankMap.at(x), suit));
							}

							if (isFound) {
								prev_ss = cur_ss;
								continue;
							}

							//if (ss_res[cur_index].find())
							bool is_pair_before_prev = (x - 1 - 1) == y;
							bool is_pair_left = (x - 1) == y;
							if (is_pair_before_prev)
							{
								ss_res[cur_index] = std::format("{}{}{}+", rankMap.at(y), rankMap.at(x), suit);
							}
							else
							{
								ss_res[cur_index].append(std::format("-{}{}{}", rankMap.at(y), rankMap.at(x), suit));
							}
						}
						else
						{
							auto temp_str = std::format("{}{}{}", rankMap.at(y), rankMap.at(x), suit);
							ss_res.push_back(temp_str);
						}
					}
					prev_ss = cur_ss;
				}
			}
		}



		for (int x = 0; x < nMax; ++x)
		{
			for (int y = 0; y < nMax; ++y)
			{
				if ((bool)arr[13 * y + x] == true)
				{
					std::string suit = "";
					if (x < y) 
					{
						suit = "o";
						int cur_os = 13 * x + y;
						
						
						continue;
					}
					else if (x != y) 
					{
						suit = "s";
						

						//prev_ss = cur_ss;
						continue;
					}
					else if (x == y)
					{
						// handling pocket pairs
						// very ugly but working
						int cur_pp = x;

						if (cur_pp != prev_pp)
						{
							if (pair_res.empty() && cur_pp == 0)
							{
								pair_res = std::format("{}{}", rankMap.at(x), rankMap.at(x));
							}

							if (cur_pp == prev_pp + 1)
							{
								//if (prev_pp == 0 || pair_res.find('+') != std::string::npos)
								if (last_pairs.empty())
									pair_res = std::format("{}{}+", rankMap.at(x), rankMap.at(x));
								else
								{
									auto temp_str = std::format("{}{}", rankMap.at(x), rankMap.at(x));
									auto& str = last_pairs[last_pairs.size() - 1];
									if (str.find('-') == std::string::npos)
										str.append("-").append(temp_str);
									else
									{
										str.erase(str.begin() + str.find('-'), str.end());
										str.append("-").append(temp_str);
									}
								}
							}
							else if (cur_pp != 0)
							{
								auto temp_str = std::format("{}{}", rankMap.at(x), rankMap.at(x));
								last_pairs.push_back(temp_str);
							}
							prev_pp = cur_pp;
						}
						continue;
					}

					result.append(std::format("{}{}{}{}", separator, rankMap.at(y), rankMap.at(x), suit));
					separator = ",";
				}
			}
		}

		separator = "";
		if (!last_pairs.empty())
		{
			if (!pair_res.empty())
				separator = ",";
			for (auto&& pair : last_pairs)
			{
				pair_res.append(separator).append(pair);
				separator = ",";
			}
		}
		separator = "";
		if (!pair_res.empty())
			separator = ",";

		for (auto&& range : ss_res)
		{
			pair_res.append(separator).append(range);
			separator = ",";
		}

		separator = "";
		if (!pair_res.empty())
			separator = ",";

		for (auto&& range : os_res)
		{
			pair_res.append(separator).append(range);
			separator = ",";
		}

		if (!result.empty())
			pair_res += ",";
		result.insert(0, pair_res);
		std::cout << result << std::endl;
		return result;
	}

	enum CARD_RANK {
		Ace = 13,
		King = 12,
		Queen = 11,
		Jack = 10,
		TEN = 9,
		NINE = 8,
		EIGHT = 7,
		SEVEN = 6,
		SIX = 5,
		FIVE = 4,
		FOUR = 3,
		THREE = 2,
		TWO = 1,
	};

	int CardToEnum(char card)
	{
		switch (card)
		{
		case 'A': return CARD_RANK::Ace;
		case 'K': return CARD_RANK::King;
		case 'Q': return CARD_RANK::Queen;
		case 'J': return CARD_RANK::Jack;
		case 'T': return CARD_RANK::TEN;
		case '9': return CARD_RANK::NINE;
		case '8': return CARD_RANK::EIGHT;
		case '7': return CARD_RANK::SEVEN;
		case '6': return CARD_RANK::SIX;
		case '5': return CARD_RANK::FIVE;
		case '4': return CARD_RANK::FOUR;
		case '3': return CARD_RANK::THREE;
		case '2': return CARD_RANK::TWO;
		default:
			break;
		}
	}

	char EnumToCard(int rank)
	{
		switch (rank)
		{
		case Ace: return 'A';
			break;
		case King:return 'K';
			break;
		case Queen:return 'Q';
			break;
		case Jack:return 'J';
			break;
		case TEN:return 'T';
			break;
		case NINE:return '9';
			break;
		case EIGHT:return '8';
			break;
		case SEVEN:return '7';
			break;
		case SIX:return '6';
			break;
		case FIVE:return '5';
			break;
		case FOUR:return '4';
			break;
		case THREE:return '3';
			break;
		case TWO:return '2';
			break;
		default:
			break;
		}
	}

	std::string RebuildDashedRange(const std::string& range)
	{
		std::string result;
		auto dashPos = range.find("-");
		auto posSuit = range.find("o") & range.find("s");
		bool isPairRange = (posSuit == std::string::npos) ? true : false;
		const char* separator = "";
		if (isPairRange)
		{
			auto first_card = range[0];
			auto card1 = CardToEnum(range[1]);
			auto card2 = CardToEnum(range[range.size() - 1]);

			for (auto card = card1; card >= card2; --card)
			{
				result.append(std::format("{}{}{}", separator, first_card, EnumToCard(card)));
				separator = ",";
			}
		}
		else
		{
			auto first_card = range[0];
			auto suit = range[posSuit];
			auto card1 = CardToEnum(range[1]);
			auto card2 = CardToEnum(range[range.size() - 1 - 1]);

			for (auto card = card1; card >= card2; --card)
			{
				result.append(std::format("{}{}{}{}", separator, first_card, EnumToCard(card), suit));
				separator = ",";
			}
			
		}
		return result;
	}

	std::string RebuildRangeString(const std::string& def_range)
	{
		std::string range = def_range;
		std::string result;
		std::string delimiter = ",";

		size_t pos = 0;
		std::string token;
		const char* separator = "";
		while ((pos = range.find(delimiter)) != std::string::npos) {
			token = range.substr(0, pos);

			if (token.find("-") != std::string::npos)
				token = RebuildDashedRange(token);

			result.append(separator).append(token);
			range.erase(0, pos + delimiter.length());
			separator = ",";
		}
		if (pos == std::string::npos && result.empty() && range.find("-") != std::string::npos)
			result = RebuildDashedRange(range);

		std::cout << range << std::endl;
		return result;
	}

	struct CardMatrix {
		bool cards[169] = {};
		std::string range;
		auto operator<=>(const CardMatrix&) const = default;
	};
	
	void RangeSelect(CardMatrix& matrix)
	{
		auto& style = ImGui::GetStyle();
		auto& io = ImGui::GetIO();
		static 	const std::unordered_map<int, char> rankMap = {
			{0, 'A'}, {1, 'K'}, {2, 'Q'}, {3, 'J'}, {4, 'T'}, {5, '9'},
			{6, '8'}, {7, '7'},   {8, '6'}, {9, '5'}, {10, '4'},  {11, '3'}, {12, '2'}
		};

		auto& selected = matrix.cards;
		//static bool selected[13 * 13] = {};
		static int start_pos = -1;
		static bool started_action = false;
		static int started_pos = -1;
		int cur_pos = -1;

		enum ACTION_TYPE {
			UNSELECTED,
			SELECTED,
		};

		static auto action_type = ACTION_TYPE::UNSELECTED;

		std::string str;
		auto posCurCursor = ImGui::GetCursorPos();
		auto posWindow = ImGui::GetWindowPos();
		auto elSize = ImVec2(40, 40);

		for (int x = 0; x < 13; ++x)
		{
			for (int y = 0; y < 13; ++y)
			{
				bool cur_state = selected[13 * x + y];
				//ImVec2 alignment = ImVec2((float)x / 2.0f, (float)y / 2.0f);
				std::string postfix = "";
				ImU32 bg_color = {};

				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.501, 0, 0.501, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.501, 0, 0.501, 1.00f));
				if (x > y)
				{
					postfix = "o";
					//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.184, 0.019, 1, 1.f));
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.862, 0.078, 0.235, 0.55f));
					//ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.26f, 0.59f, 0.98f, 0.80f));
					//ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.501, 0, 0.501, 1.00f));
					bg_color = IM_COL32(255, 160, 122, 200);
				}
				else if (x != y)
				{
					postfix = "s";
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.24f, 0.27f, 0.20f, 1.00f));
					//ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.27f, 0.30f, 0.23f, 1.00f));
					//ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.30f, 0.34f, 0.26f, 1.00f));
					bg_color = IM_COL32(255, 218, 185, 200);
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0.501, 0.82f));
					//ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.603, 0.803, 0.196, 0.83f));
					//ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.678f, 1.f, 0.184f, 0.86f));
					bg_color = IM_COL32(135, 206, 235, 200);
				}
				auto first = rankMap.at(x);
				auto second = rankMap.at(y);
				if (y < x)
					std::swap(first, second);

				str = std::format("{}{}{}", first, second, postfix);
				auto flags = ImGuiSelectableFlags_SelectOnClick;
				auto prevCurPos = ImGui::GetCursorScreenPos();
				if (widgets::CustomSelectable(str.data(), &selected[13 * x + y], bg_color, flags, ImVec2(40, 40)))
				{
					bool isShiftPressed = io.KeyShift;
					// Imgui's 'IsKeyPressed' doesn't work properly somewhy
					// For windows i can use 'GetAsyncKeyState'
					// But to keep cross-platform i can use io.KeyShift
					if (isShiftPressed && (y == x || y + x == 13 * 13 - 1))
						//if (GetAsyncKeyState(VK_SHIFT) & 0x8000 && (y == x || y + x == 13 * 13 - 1)) // pocket pairs
					{
						//for (int k = x + 1; k < 13; ++k)
						for (int k = x - 1; k >= 0; --k)
						{
							auto& cur_val = selected[13 * k + k];
							if (cur_state && cur_val)
								cur_val ^= 1;
							else if (!cur_state && !cur_val)
								cur_val ^= 1;
						}
					}
					if (isShiftPressed && x > y) // off-suited
					{
						for (int k = x - 1; k >= 0; --k)
						{
							if (y >= k) continue;

							auto& cur_val = selected[13 * k + y];
							if (cur_state && cur_val)
								cur_val ^= 1;
							else if (!cur_state && !cur_val)
								cur_val ^= 1;
						}

					}
					if (isShiftPressed && x < y)	// single-suited
					{
						for (int k = y - 1; k >= 0; --k)
						{
							if (x >= k) continue;
							auto& cur_val = selected[13 * x + k];
							if (cur_state && cur_val)
								cur_val ^= 1;
							else if (!cur_state && !cur_val)
								cur_val ^= 1;
						}
					}


					started_action = true;
					if (cur_state == false)
						action_type = ACTION_TYPE::UNSELECTED;
					else
						action_type = ACTION_TYPE::SELECTED;
				}
				if (!ImGui::IsItemHovered() && !selected[13 * x + y])
					if (x > y)
					{
						//SelectableColor(elSize, IM_COL32(255, 0, 0, 200));
						//SelectableColorEx(ImVec2(x, y), posCurCursor + posWindow, elSize, IM_COL32(255, 0, 0, 200));
						//SelectableColorEx2(prevCurPos, elSize, IM_COL32(255, 0, 0, 200));
					}

				if (y != 13 - 1) ImGui::SameLine();
				ImGui::PopStyleColor(3);
			}
		}

		posCurCursor = posCurCursor;
		posWindow = posWindow;
		
		for (int x = 0; x < 13; ++x)
		{
			for (int y = 0; y < 13; ++y)
			{
				ImVec2 min = posCurCursor + posWindow;
				auto itemSpacing = style.ItemSpacing;
				min.x += x * 40 + x * itemSpacing.x;
				min.y += y * 40 + y * itemSpacing.y;
				ImVec2 max = min + elSize + style.ItemSpacing;

				if (ImGui::IsMouseHoveringRect(min, max))
				{
					cur_pos = 13 * y + x;
				}
			}
		}


		bool isDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);
		if (started_action && isDragging)
		{
			if (cur_pos != -1)
			{
				if (action_type == ACTION_TYPE::UNSELECTED)
					selected[cur_pos] = true;
				else
					selected[cur_pos] = false;
			}
		}

		static std::string str_range = "Empty range";
		if (started_action && !io.MouseDown[0])
		{
			started_action = false;

			auto str_data = BuildRangeString(selected);
			matrix.range = str_data;
			if (!str_data.empty())
				str_range = str_data;
			else
				str_range = "Empty range";
		}

		
		if (ImGui::Button("Clear all"))
			std::fill(selected, selected + 169, 0);
		ImGui::SameLine();
		if (ImGui::Button("Generate range"))
		{
			auto str_data = BuildRangeString(selected);
			matrix.range = str_data;
			if (!str_data.empty())
				str_range = str_data;
		}
		ImGui::Text(str_range.data());


	}

	template<size_t N>
	std::string BuildBoardString(const std::array<std::string, N>& board)
	{
		std::string result;
		for (auto&& card : board)
		{
			result.append(card);
		}
		return result;
	}

	void BoardSelect()
	{
		const std::unordered_map<int, char> rankMap = {
		{0, 'A'}, {1, 'K'}, {2, 'Q'}, {3, 'J'}, {4, 'T'}, {5, '9'},
		{6, '8'}, {7, '7'},   {8, '6'}, {9, '5'}, {10, '4'},  {11, '3'}, {12, '2'}
		};
		const char suits[4] = { 'h', 'c', 'd', 's' };

		constexpr auto holdem_max_board_cards = 5;
		//std::string boards_cards[holdem_max_board_cards] = {};
		static std::array<std::string, holdem_max_board_cards> boards_cards = {};
		//static std::vector<std::string> board_cards = {};
		static size_t szArr = 0;
		auto szButton = ImVec2(20, 20);

		ImGui::Text("Board Select:");
		bool separator = false;
		for (auto&& card : boards_cards)
		{
			if (separator) ImGui::SameLine();
			ImGui::Button(card.data(), szButton);
			separator = true;
		}



		static bool selectables[13 * 4] = {};
		separator = false;
		static bool are_disabled = false;

		for (int y = 0; y < 13; ++y)
		{
			auto& card = rankMap.at(y);
			for (int x = 0; x < 4; ++x)
			{
				auto& suit = suits[x];
				if (separator) ImGui::SameLine();
				auto label = std::format("{}{}", card, suit);

				bool disabled_helper = are_disabled && !(selectables[13 * x + y]);
				ImGui::BeginDisabled(disabled_helper);
				auto flags = ImGuiSelectableFlags_SelectOnClick;
				auto prev_value = selectables[13 * x + y];
				auto& cur_value = selectables[13 * x + y];
				if (ImGui::Selectable(label.data(), &selectables[13 * x + y], flags, ImVec2(40, 40)))
				{
					bool value_changed = prev_value && !cur_value;
					auto label_prev = std::format("{}{}", card, suit);
					if (value_changed)
					{
						--szArr;
						for (int k = 0; k < boards_cards.size(); ++k)
						{
							auto& cur_str = boards_cards[k];
							if (cur_str.find(label_prev) != std::string::npos)
							{
								cur_str = "";
								for (int ijk = k; ijk < boards_cards.size() - 1; ++ijk)
								{
									boards_cards[ijk] = boards_cards[ijk + 1];
								}
								boards_cards[szArr] = "";
							}
						}
					}
					else if (szArr != boards_cards.size())
						boards_cards[szArr++] = label;
				}
				ImGui::EndDisabled();
				separator = true;
			}
			separator = false;
		}

		if (szArr == boards_cards.size())
			are_disabled = true;
		else
			are_disabled = false;

		if (ImGui::Button("Clear##Board"))
		{
			boards_cards.fill("");
			szArr = 0;
			are_disabled = false;
			std::fill(std::begin(selectables), std::end(selectables), 0);
		}

	}

	struct Cards {
		bool cards[52] = {};
		auto operator<=>(const Cards&) const = default;
	};

	auto GetBoardCardsText(const Cards& cards) -> std::vector<std::string>
	{
		const std::unordered_map<int, char> rankMap = {
		   {0, 'A'}, {1, 'K'}, {2, 'Q'}, {3, 'J'}, {4, 'T'}, {5, '9'},
		   {6, '8'}, {7, '7'},   {8, '6'}, {9, '5'}, {10, '4'},  {11, '3'}, {12, '2'}
		};
		const char suits[4] = { 'h', 'c', 'd', 's' };

		std::vector<std::string> result;
		const auto& selectables = cards.cards;
		for (int y = 0; y < 13; ++y)
		{
			auto& card = rankMap.at(y);
			for (int x = 0; x < 4; ++x)
			{
				auto& suit = suits[x];
				if (selectables[13 * x + y])
					result.push_back(std::format("{}{}", card, suit));
			}
		}
		return result;
	}

	std::string BuildBoardString(const Cards& cards)
	{
		std::string result = "";
		auto board = GetBoardCardsText(cards);
		
		for (auto&& card : board)
		{
			result.append(card);
		}

		return result;
	}

	template<size_t max_selected = 5>
	std::string CardSelection(Cards& cards, bool sync, Cards& another_cards)
	{
		const std::unordered_map<int, char> rankMap = {
		{0, 'A'}, {1, 'K'}, {2, 'Q'}, {3, 'J'}, {4, 'T'}, {5, '9'},
		{6, '8'}, {7, '7'},   {8, '6'}, {9, '5'}, {10, '4'},  {11, '3'}, {12, '2'}
		};
		const char suits[4] = { 'h', 'c', 'd', 's' };
		auto& selectables		= cards.cards;
		auto& other_selectables = another_cards.cards;
		std::array<std::string, max_selected> hero_hand = {};
		size_t szArr = std::count(std::begin(selectables), std::end(selectables), 1);
		size_t szAnotArr = std::count(std::begin(other_selectables), std::end(other_selectables), 1);
		bool are_disabled = (szArr == hero_hand.size()) ? true : false;
		bool separator = false;

		for (int y = 0; y < 13; ++y)
		{
			auto& card = rankMap.at(y);
			for (int x = 0; x < 4; ++x)
			{
				auto& suit = suits[x];
				ImU32 bg_color = {};
				switch (suit)
				{
				case 'h': bg_color = IM_COL32(240, 128, 128, 200); break;
				case 'c': bg_color = IM_COL32(50, 205, 50, 200); break;
				case 'd': bg_color = IM_COL32(135, 206, 250, 200); break;
				case 's': bg_color = IM_COL32(105, 105, 105, 200); break;
				default:
					break;
				}

				if (separator) ImGui::SameLine();
				auto label = std::format("{}{}", card, suit);

				bool disabled_helper = are_disabled && !(selectables[13 * x + y]) || (sync && other_selectables[13 * x + y]);
				ImGui::BeginDisabled(disabled_helper);
				auto flags = ImGuiSelectableFlags_SelectOnClick;
				auto prev_value = selectables[13 * x + y];
				auto& cur_value = selectables[13 * x + y];
				if (CustomSelectable(label.data(), &selectables[13 * x + y], bg_color, flags, ImVec2(40, 40)))
				{
					bool value_changed = prev_value && !cur_value;
					auto label_prev = std::format("{}{}", card, suit);
					if (value_changed)
					{
						--szArr;
						for (int k = 0; k < hero_hand.size(); ++k)
						{
							auto& cur_str = hero_hand[k];
							if (cur_str.find(label_prev) != std::string::npos)
							{
								cur_str = "";
								for (int ijk = k; ijk < hero_hand.size() - 1; ++ijk)
								{
									hero_hand[ijk] = hero_hand[ijk + 1];
								}
								hero_hand[szArr] = "";
							}
						}
					}
					else if (szArr != hero_hand.size())
						hero_hand[szArr++] = label;
				}
				ImGui::EndDisabled();
				separator = true;
			}
			separator = false;

		}

		std::string result;


		return result;
	}

	template<size_t max_selected = 5>
	void DeadCardsSelect(Cards& cards, const std::string& range = "random", bool sync = false, Cards& board = Cards())
	{
		ImGui::Text("Dead Cards:");
		CardSelection<max_selected>(cards, sync, board);
		auto result = GetBoardCardsText(cards);
		static auto prevRange = range;
		static auto prevCards = cards;
		static auto prevBoard = board;
		static bool changed_values = false;

		auto& selectables = cards.cards;
		if (ImGui::Button("Clear##Dead Cards"))
		{
			std::fill(std::begin(selectables), std::end(selectables), 0);
		}

		bool bHeroHandSelected = result.size() == max_selected;
		bool board_changed = GetBoardCardsText(board).size() >= 3 && prevBoard != board;
		if (bHeroHandSelected && (prevRange != range || prevCards != cards || board_changed))
		{
			changed_values = true;
			prevRange = range;
			prevCards = cards;
			prevBoard = board;
		}

		//static bool calculated = false;
		static std::string result_str;

		std::string output = "Enter two holecards to see their equity vs the range";
		if (bHeroHandSelected && changed_values)
		{
			using namespace omp;
			auto hero_hand = BuildBoardString(cards);
			auto board_cards = BuildBoardString(board);
			auto rebuilt_range = RebuildRangeString(range);
			std::vector<CardRange> ranges2{ rebuilt_range, hero_hand };
			omp::EquityCalculator equity;
			uint64_t board_mask = CardRange::getCardMask(board_cards);
			uint64_t dead_cards = CardRange::getCardMask(hero_hand);
			
			equity.start(ranges2, board_mask, 0, true, 0, nullptr, 0.05f, 0);
			equity.wait();
			auto res = equity.getResults();
			std::cout << "Number of " << res.evaluations << std::endl;
			output =
				"Equity: {:#3.3f} %%\n"
				"Win:    {:#3.3f} %%\n"
				"Tie:    {:#3.3f} %%";
			auto total_wins = res.wins[0] + res.wins[1];
			auto total_ties = res.ties[0] + res.ties[1];
			auto total = total_ties + total_wins;
			float eq_percent = res.equity[1] * 100.f;
			float wins_percent = res.wins[1] / (float)total * 100.f;
			float ties_percent = res.ties[1] / (float)total * 100.f;
			result_str = std::format(output, eq_percent, wins_percent, ties_percent);
			changed_values = false;
		}

	

		float wrap_width = 200.f;
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 marker_min = ImVec2(pos.x + wrap_width, pos.y);
		ImVec2 marker_max = ImVec2(pos.x + wrap_width + 10, pos.y + ImGui::GetTextLineHeight());
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
		ImGui::Text(result_str.empty() ? output.data() : result_str.data());
	}

	template<size_t max_selected = 5>
	void BoardSelect(Cards& cards, bool sync = false, Cards& board = Cards())
	{
		auto boards_cards = GetBoardCardsText(cards);
		auto& selectables = cards.cards;
		ImGui::Text("Board Select:");
		bool separator = false;
		auto szButton = ImVec2(20, 20);

		for (int i = 0; i < max_selected; ++i)
		{
			std::string label = (boards_cards.size() > i) ? boards_cards[i] : "";
			if (separator) ImGui::SameLine();
			ImGui::Button(label.data(), szButton);
			separator = true;
		}

		CardSelection<max_selected>(cards, sync, board);

		if (ImGui::Button("Clear##Board"))
		{
			std::fill(std::begin(selectables), std::end(selectables), 0);
		}
	}

	void DeadCardsSelect()
	{
		const std::unordered_map<int, char> rankMap = {
		{0, 'A'}, {1, 'K'}, {2, 'Q'}, {3, 'J'}, {4, 'T'}, {5, '9'},
		{6, '8'}, {7, '7'},   {8, '6'}, {9, '5'}, {10, '4'},  {11, '3'}, {12, '2'}
		};
		const char suits[4] = { 'h', 'c', 'd', 's' };

		constexpr auto max_hero_cards = 2;
		static std::array<std::string, max_hero_cards> hero_hand = {};
		static size_t szArr = 0;

		static bool selectables[13 * 4] = {};
		bool separator = false;
		static bool are_disabled = false;

		ImGui::Text("Dead Cards:");
		for (int y = 0; y < 13; ++y)
		{
			auto& card = rankMap.at(y);
			for (int x = 0; x < 4; ++x)
			{
				auto& suit = suits[x];
				if (separator) ImGui::SameLine();
				auto label = std::format("{}{}", card, suit);

				bool disabled_helper = are_disabled && !(selectables[13 * x + y]);
				ImGui::BeginDisabled(disabled_helper);
				auto flags = ImGuiSelectableFlags_SelectOnClick;
				auto prev_value = selectables[13 * x + y];
				auto& cur_value = selectables[13 * x + y];
				if (ImGui::Selectable(label.data(), &selectables[13 * x + y], flags, ImVec2(40, 40)))
				{
					bool value_changed = prev_value && !cur_value;
					auto label_prev = std::format("{}{}", card, suit);
					if (value_changed)
					{
						--szArr;
						for (int k = 0; k < hero_hand.size(); ++k)
						{
							auto& cur_str = hero_hand[k];
							if (cur_str.find(label_prev) != std::string::npos)
							{
								cur_str = "";
								for (int ijk = k; ijk < hero_hand.size() - 1; ++ijk)
								{
									hero_hand[ijk] = hero_hand[ijk + 1];
								}
								hero_hand[szArr] = "";
							}
						}
					}
					else if (szArr != hero_hand.size())
						hero_hand[szArr++] = label;
				}
				ImGui::EndDisabled();
				separator = true;
			}
			separator = false;
		}

		if (szArr == hero_hand.size())
			are_disabled = true;
		else
			are_disabled = false;

		if (ImGui::Button("Clear##Dead Cards"))
		{
			hero_hand.fill("");
			szArr = 0;
			are_disabled = false;
			std::fill(std::begin(selectables), std::end(selectables), 0);
		}

		std::string output = "Enter two holecards to see their equity vs the range";
		if (szArr == hero_hand.size())
		{
			output =
				"Equity: 50 %%\n"
				"Win:    45 %%\n"
				"Tie:    5 %%";
		}

		float wrap_width = 200.f;
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 marker_min = ImVec2(pos.x + wrap_width, pos.y);
		ImVec2 marker_max = ImVec2(pos.x + wrap_width + 10, pos.y + ImGui::GetTextLineHeight());
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
		ImGui::Text(output.data());
	}

	void TreeRangeEditor()
	{
		// Buttons to add/delete headers
		const char* edit_buttons[] = { ICON_FA_PLUS_CIRCLE, ICON_FA_FILE, ICON_FA_LIST };
		size_t sz_Arr = ARRAYSIZE(edit_buttons);

		bool separator = false;
		for (int i = 0; i < sz_Arr; ++i)
		{
			if (separator) ImGui::SameLine();
			ImGui::Button(edit_buttons[i]);
			separator = true;
		}

		ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_SpanFullWidth;
		if (ImGui::CollapsingHeader("Default Ranges", ImGuiTreeNodeFlags_None))
		{
			int node_clicked = -1;
			const char* leafs[] = { "OR UTG-1", "OR CO", "OR BU" };
			static int selection_mask = (1 << 0);

			for (int i = 0; i < 3; ++i)
			{
				ImGuiTreeNodeFlags node_flags = base_flags;
				const bool is_selected = (selection_mask & (1 << i)) != 0;
				if (is_selected)
					node_flags |= ImGuiTreeNodeFlags_Selected;
				node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

				ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, leafs[i], i);
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					node_clicked = i;
			}

			if (node_clicked != -1)
			{
				// Update selection state
				// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
				if (ImGui::GetIO().KeyCtrl)
					selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
				else if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
					selection_mask = (1 << node_clicked);           // Click to single-select
			}

		}
		if (ImGui::CollapsingHeader("NL2-NL10 Ranges", ImGuiTreeNodeFlags_None))
		{
			constexpr auto nFilters = 6;
			const char* filters[nFilters] = { "Open-Raise", "Call OR", "3-Bet", "Call 3-Bet", "4-Bet", "5-Bet - Push" };

			for (int i = 0; i < nFilters; ++i)
			{
				auto curPos = ImGui::GetCursorScreenPos();
				curPos.x += 20.f;
				ImGui::SetCursorScreenPos(curPos);
				if (ImGui::CollapsingHeader(filters[i], ImGuiTreeNodeFlags_None))
				{
					int node_clicked = -1;
					const char* leafs[] = { "OR UTG-1", "OR CO", "OR BU" };
					static int selection_mask = (1 << 0);

					for (int i = 0; i < 3; ++i)
					{
						ImGuiTreeNodeFlags node_flags = base_flags;
						const bool is_selected = (selection_mask & (1 << i)) != 0;
						if (is_selected)
							node_flags |= ImGuiTreeNodeFlags_Selected;
						node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

						ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, leafs[i], i);
						if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
							node_clicked = i;
					}

					if (node_clicked != -1)
					{
						// Update selection state
						// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
						if (ImGui::GetIO().KeyCtrl)
							selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
						else if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
							selection_mask = (1 << node_clicked);           // Click to single-select
					}
				}
			}
		}
		if (ImGui::CollapsingHeader("NL10-NL25 Ranges", ImGuiTreeNodeFlags_None))
		{
			constexpr auto nSzArr = 3;
			static bool selection[nSzArr] = { false, false, false };
			const char* leafs[nSzArr] = { "UTG-1 3-Bet", "BB 3-Bet", "SB 3-Bet" };

			for (int n = 0; n < nSzArr; n++)
			{
				if (ImGui::Selectable(leafs[n], selection[n]))
				{
					//if (!ImGui::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
					memset(selection, 0, sizeof(selection));
					selection[n] ^= 1;
				}
			}
		}
		if (ImGui::CollapsingHeader("NL25-100 Ranges", ImGuiTreeNodeFlags_None))
		{
			ImGui::Text("IsItemHovered: %d", ImGui::IsItemHovered());
			for (int i = 0; i < 5; i++)
				ImGui::Text("Some content %d", i);
		}
		if (ImGui::CollapsingHeader("MTT Ranges (ABI 1)", ImGuiTreeNodeFlags_None))
		{
			ImGui::Text("IsItemHovered: %d", ImGui::IsItemHovered());
			for (int i = 0; i < 5; i++)
				ImGui::Text("Some content %d", i);
		}
		if (ImGui::CollapsingHeader("MTT Ranges (ABI 10)", ImGuiTreeNodeFlags_None))
		{
			ImGui::Text("IsItemHovered: %d", ImGui::IsItemHovered());
			for (int i = 0; i < 5; i++)
				ImGui::Text("Some content %d", i);
		}
		if (ImGui::CollapsingHeader("MTT Ranges (ABI 50)", ImGuiTreeNodeFlags_None))
		{
			ImGui::Text("IsItemHovered: %d", ImGui::IsItemHovered());
			for (int i = 0; i < 5; i++)
				ImGui::Text("Some content %d", i);
		}

	}

}

#endif // !SOURCE_CUSTOM_WIDGETS_HPP