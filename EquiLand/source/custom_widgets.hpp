#ifndef SOURCE_CUSTOM_WIDGETS_HPP
#define SOURCE_CUSTOM_WIDGETS_HPP

#include <includes_pch.h>

#include "OMPEval/HandEvaluator.h"
#include "OMPEval/EquityCalculator.h"

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

	void SelectableColor(ImU32 color)
	{
		ImVec2 p_min = ImGui::GetItemRectMin();
		ImVec2 p_max = ImGui::GetItemRectMax();
		ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, color);
	}

	bool CustomSelectable(const char* label, bool* p_selected, ImU32 bg_color, ImGuiSelectableFlags flags, const ImVec2& size_arg)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->ChannelsSplit(2);

		// Channel number is like z-order. Widgets in higher channels are rendered above widgets in lower channels.
		draw_list->ChannelsSetCurrent(1);

		bool result = ImGui::Selectable(label, p_selected, flags, size_arg);

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
				if (arr[13 * y + x] == true)
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
				if (separator) ImGui::SameLine();
				auto label = std::format("{}{}", card, suit);

				bool disabled_helper = are_disabled && !(selectables[13 * x + y]) || (sync && other_selectables[13 * x + y]);
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
}

#endif // !SOURCE_CUSTOM_WIDGETS_HPP