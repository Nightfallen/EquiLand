#ifndef SOURCE_WIDGETS_BOARDSELECT_HPP
#define SOURCE_WIDGETS_BOARDSELECT_HPP

#include <includes_pch.h>
#include <source/widgets/CardSelection.hpp>
#include <source/structures.hpp>

namespace widgets::EquiLand {

	template<size_t max_selected = 5>
	void BoardSelect(Cards& cards, bool sync = false, Cards& board = Cards())
	{
		static std::vector<std::string> currentCards;
		auto& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();
		auto boards_cards = GetBoardCardsText(cards);
		auto& selectables = cards.cards;
		ImGui::Text("Board Select:");
		bool separator = false;

		if (currentCards.size() != boards_cards.size())
		{
			if (currentCards.size() < boards_cards.size())
			{
				for (auto&& el : boards_cards)
				{
					if (std::find(currentCards.begin(), currentCards.end(), el) == currentCards.end())
					{
						currentCards.push_back(el);
						break;
					}
				}
			}
			else if (currentCards.size() > boards_cards.size())
			{
				for (int i = 0; auto&& el : currentCards)
				{
					if (std::find(boards_cards.begin(), boards_cards.end(), el) == boards_cards.end())
					{
						currentCards.erase(currentCards.begin() + i);
						break;
					}
					++i;
				}
			}
		}

		ImGui::PushFont(io.Fonts->Fonts[(int)FONT::CARDS]);
		auto szButton = ImGui::CalcTextSize("QQ") + style.FramePadding * 2 + style.ItemInnerSpacing;
		for (int i = 0; i < max_selected; ++i)
		{
			std::string label = "";
			if (auto szVec = boards_cards.size(); szVec > i)
			{
				label = currentCards[i];
			}
			ImU32 colText = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]);
			if (!label.empty())
			{
				if (label[1] == 'h')
				{
					colText = IM_COL32(240, 128, 128, 200);
					label[1] = '{';
				}
				if (label[1] == 'd')
				{
					colText = IM_COL32(135, 206, 250, 200);
					label[1] = '[';
				}
				if (label[1] == 'c')
				{
					colText = IM_COL32(50, 205, 50, 200);
					label[1] = ']';
				}
				if (label[1] == 's')
				{
					colText = IM_COL32(105, 105, 105, 200);
					label[1] = '}';
				}
			}
			ImGui::PushStyleColor(ImGuiCol_Text, colText);

			if (separator) ImGui::SameLine();
			ImGui::Button(label.data(), szButton);
			ImGui::PopStyleColor();
			separator = true;
		}
		ImGui::PopFont();

		CardSelection<max_selected>(cards, sync, board);

		if (ImGui::Button("Clear##Board"))
		{
			std::fill(std::begin(selectables), std::end(selectables), 0);
		}
	}
}

#endif // !SOURCE_WIDGETS_BOARDSELECT_HPP