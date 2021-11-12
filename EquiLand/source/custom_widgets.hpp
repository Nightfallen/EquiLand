#ifndef SOURCE_CUSTOM_WIDGETS_HPP
#define SOURCE_CUSTOM_WIDGETS_HPP

#include <includes_pch.h>

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


}

#endif // !SOURCE_CUSTOM_WIDGETS_HPP