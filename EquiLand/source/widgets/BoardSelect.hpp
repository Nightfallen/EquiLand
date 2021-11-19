#ifndef SOURCE_WIDGETS_BOARDSELECT_HPP
#define SOURCE_WIDGETS_BOARDSELECT_HPP

#include <includes_pch.h>
#include <source/widgets/CardSelection.hpp>

namespace widgets::EquiLand {

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
}

#endif // !SOURCE_WIDGETS_BOARDSELECT_HPP