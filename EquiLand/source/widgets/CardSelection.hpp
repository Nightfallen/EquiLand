#ifndef SOURCE_WIDGETS_CARDSELECTION_HPP
#define SOURCE_WIDGETS_CARDSELECTION_HPP

#include <includes_pch.h>
#include <source/widgets/BasicCustomWidgets.hpp>
#include <source/utils/ColorHelpers.hpp>

namespace widgets::EquiLand {

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

	template<size_t max_selected = 5>
	std::string CardSelection(Cards& cards, bool sync, Cards& another_cards)
	{
		const std::unordered_map<int, char> rankMap = {
		{0, 'A'}, {1, 'K'}, {2, 'Q'}, {3, 'J'}, {4, 'T'}, {5, '9'},
		{6, '8'}, {7, '7'},   {8, '6'}, {9, '5'}, {10, '4'},  {11, '3'}, {12, '2'}
		};
		const char suits[4] = { 'h', 'c', 'd', 's' };
		auto& selectables = cards.cards;
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

				ImGui::PushStyleColor(ImGuiCol_Header, helpers::BrightenColor(bg_color, 15));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.501, 0, 0.501, 1.00f));
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
				ImGui::PopStyleColor(2);
				ImGui::EndDisabled();
				separator = true;
			}
			separator = false;
		}

		std::string result;
		return result;
	}
}

#endif // !SOURCE_WIDGETS_CARDSELECTION_HPP 