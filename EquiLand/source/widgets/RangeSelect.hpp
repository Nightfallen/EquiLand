#ifndef SOURCE_WIDGETS_RANGESELECT_HPP
#define SOURCE_WIDGETS_RANGESELECT_HPP

#include <includes_pch.h>
#include <source/widgets/BasicCustomWidgets.hpp>

namespace widgets::EquiLand {

	struct CardMatrix {
		bool cards[169] = {};
		std::string range;
		auto operator<=>(const CardMatrix&) const = default;
	};

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

}

#endif // !SOURCE_WIDGETS_RANGESELECT_HPP