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

	void SelectableColor(ImVec2 szSelectable, ImU32 color)
	{
		auto& style = ImGui::GetStyle();
		ImVec2 itemSpacing = style.ItemSpacing;
		ImVec2 p_min = ImGui::GetCursorScreenPos() - style.FramePadding;
		// temp workaround to 
		p_min.y += 1;
		ImVec2 p_max = p_min + itemSpacing + szSelectable; //ImVec2(p_min.x + ImGui::GetContentRegionAvailWidth(), p_min.y + ImGui::GetFrameHeight());
		ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, color);
		
	}

	void SelectableColorEx2(ImVec2 prevCurPos, ImVec2 szSelectable, ImU32 color)
	{
		auto& style = ImGui::GetStyle();
		ImVec2 itemSpacing = style.ItemSpacing;
		ImVec2 p_min = prevCurPos - style.FramePadding;
		// temp workaround to 
		p_min.y += 1;
		ImVec2 p_max = p_min + itemSpacing + szSelectable; //ImVec2(p_min.x + ImGui::GetContentRegionAvailWidth(), p_min.y + ImGui::GetFrameHeight());
		//ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, color);
		//ImGui::GetBackgroundDrawList()->AddRectFilled(p_min, p_max, color);
		ImGui::GetForegroundDrawList()->AddRectFilled(p_min, p_max, color);
	}

	void SelectableColorEx(ImVec2 xy, ImVec2 min, ImVec2 szSelectable, ImU32 color)
	{
		auto& style = ImGui::GetStyle();
		auto& [x, y] = xy;
		ImVec2 p_min = min;
		auto itemSpacing = style.ItemSpacing;
		p_min.x += y * 40 + y * itemSpacing.y;
		p_min.y += x * 40 + x * itemSpacing.x;
		ImVec2 p_max = p_min + szSelectable + style.ItemSpacing;
		//ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, color);
		ImGui::GetBackgroundDrawList()->AddRectFilled(p_min, p_max, color);
	}

	void RangeSelect()
	{
		auto& style = ImGui::GetStyle();
		auto& io = ImGui::GetIO();
		static 	const std::unordered_map<int, char> rankMap = {
			{0, 'A'}, {1, 'K'}, {2, 'Q'}, {3, 'J'}, {4, 'T'}, {5, '9'},
			{6, '8'}, {7, '7'},   {8, '6'}, {9, '5'}, {10, '4'},  {11, '3'}, {12, '2'}
		};


		static bool selected[13 * 13] = {};
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
				if (x > y)
				{
					postfix = "o";
					//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.184, 0.019, 1, 1.f));
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.20f, 0.25f, 0.29f, 0.55f));
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.26f, 0.59f, 0.98f, 0.80f));
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.26f, 0.59f, 0.98f, 1.00f));


				}
				else if (x != y)
				{
					postfix = "s";
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.24f, 0.27f, 0.20f, 1.00f));
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.27f, 0.30f, 0.23f, 1.00f));
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.30f, 0.34f, 0.26f, 1.00f));
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.00f, 0.00f, 0.00f, 0.52f));
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.00f, 0.00f, 0.00f, 0.36f));
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.20f, 0.22f, 0.23f, 0.33f));
				}
				auto first = rankMap.at(x);
				auto second = rankMap.at(y);
				if (second > first)
					std::swap(first, second);

				str = std::format("{}{}{}", first, second, postfix);
				auto flags = ImGuiSelectableFlags_SelectOnClick;
				auto prevCurPos = ImGui::GetCursorScreenPos();
				if (ImGui::Selectable(str.data(), &selected[13 * x + y], flags, ImVec2(40, 40)))
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
						SelectableColorEx2(prevCurPos, elSize, IM_COL32(255, 0, 0, 200));
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
	}

}

#endif // !SOURCE_CUSTOM_WIDGETS_HPP