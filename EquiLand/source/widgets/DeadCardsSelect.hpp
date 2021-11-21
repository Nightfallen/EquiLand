#ifndef SOURCE_WIDGETS_DEADCARDSSELECT_HPP
#define SOURCE_WIDGETS_DEADCARDSSELECT_HPP

#include <includes_pch.h>
#include <source/widgets/CardSelection.hpp>
#include <OMPEval/HandEvaluator.h>
#include <OMPEval/EquityCalculator.h>

namespace {
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
		return -1;
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
		return -1;
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
				auto temp_vla = std::format("{}{}{}", separator, first_card, EnumToCard(card));
				result.append(temp_vla);
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
}

namespace widgets::EquiLand {
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

		constexpr const char* output = "Enter two holecards to see their equity vs the range";
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
			constexpr const char* output2 =
				"Equity: {:#3.3f} %%\n"
				"Win:    {:#3.3f} %%\n"
				"Tie:    {:#3.3f} %%";
			auto total_wins = res.wins[0] + res.wins[1];
			auto total_ties = res.ties[0] + res.ties[1];
			auto total = total_ties + total_wins;
			float eq_percent = res.equity[1] * 100.f;
			float wins_percent = res.wins[1] / (float)total * 100.f;
			float ties_percent = res.ties[1] / (float)total * 100.f;
			result_str = std::format(output2, eq_percent, wins_percent, ties_percent);
			changed_values = false;
		}



		float wrap_width = 200.f;
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 marker_min = ImVec2(pos.x + wrap_width, pos.y);
		ImVec2 marker_max = ImVec2(pos.x + wrap_width + 10, pos.y + ImGui::GetTextLineHeight());
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
		ImGui::Text(result_str.empty() ? output : result_str.data());
		if (ImGui::BeginPopupContextItem("some_id"))
		{
			if (ImGui::MenuItemEx("Copy Equity %", ICON_FA_LINK))
			{
				ImGui::SetClipboardText(result_str.empty() ? output : result_str.data());
			}
			ImGui::EndPopup();
		}
	}
}

#endif // !SOURCE_WIDGETS_DEADCARDSSELECT_HPP