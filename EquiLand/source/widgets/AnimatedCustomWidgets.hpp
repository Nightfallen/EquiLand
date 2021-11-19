#ifndef SOURCE_WIDGETS_ANIMATEDCUSTOMWIDGETS_HPP
#define SOURCE_WIDGETS_ANIMATEDCUSTOMWIDGETS_HPP

// Untested code of imgui widgets i found searching the net
// Need to rework them or discard if they won't be needed in the future
namespace widgets::public_untested {

	// I found it on the net, very good idea
	// But i need to reimplement it to understand how it works 
	// And also redesign it to not using 'Imgui::Begin()' and 'ImGui::End()' functions,
	// cause it's very confusing, there are 
	// ImGui::GetOverlayDrawList() and other ways to do the same thing
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

#endif // !SOURCE_WIDGETS_ANIMATEDCUSTOMWIDGETS_HPP