#ifndef SOURCE_GUI_HPP
#define SOURCE_GUI_HPP

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

	// Constructing pairs
	const char* separator = "";
	int nMax = std::sqrt(N);

	// Some unreachable value
	int prev_pp = 0xff;

	const int max_pairs_ranges = 7;
	std::string pair_res;
	//std::string last_pairs[max_pairs_ranges] = {};
	std::vector<std::string> last_pairs = {};
	last_pairs.reserve(7);

	for (int x = 0; x < nMax; ++x)
	{
		for (int y = 0; y < nMax; ++y)
		{
			if (arr[13 * y + x] == true)
			{
				std::string suit = "";
				if (x < y)
					suit = "o";
				else if (x != y)
					suit = "s";
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
							if (prev_pp == 0 || pair_res.find('+') != std::string::npos)
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
						else
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

	if (!result.empty())
		pair_res += ",";
	result.insert(0, pair_res);
	std::cout << result << std::endl;
	return result;
}

void RangeSelect()
{
	auto& style = ImGui::GetStyle();
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

	auto& io = ImGui::GetIO();
	std::string str;
	auto posCurCursor = ImGui::GetCursorPos();
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
			str = std::format("{}{}{}", rankMap.at(x), rankMap.at(y), postfix);
			auto flags = ImGuiSelectableFlags_SelectOnClick;
			if (ImGui::Selectable(str.data(), &selected[13 * x + y], flags, ImVec2(40, 40)))
			{
				if (GetAsyncKeyState(VK_SHIFT) & 0x8000 && (y == x || y + x == 13 * 13 - 1))
				{
					//for (int k = x + 1; k < 13; ++k)
					for (int k = x - 1; k >= 0; --k)
					{
						selected[13 * k + k] = !selected[13 * k + k];
					}
				}

				started_action = true;
				if (cur_state == false)
					action_type = ACTION_TYPE::UNSELECTED;
				else
					action_type = ACTION_TYPE::SELECTED;
			}
			if (y != 13 - 1) ImGui::SameLine();
			ImGui::PopStyleColor(3);
		}
	}

	posCurCursor = posCurCursor;
	auto posWindow = ImGui::GetWindowPos();
	auto elSize = ImVec2(40, 40);
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

	if (started_action && !io.MouseDown[0])
	{
		started_action = false;
	}

	static std::string str_range = "Empty range";
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

void CustomWindow(bool* is_open)
{
	ImGui::Begin("EquiLand");

	ImGui::Text("Something coming soon...");
	RangeSelect();
	ImGui::End();
}

void UI_HANDLER(HWND hwnd)
{
	// Our state
	static bool show_demo_window = true;
	static bool show_equiland_window = true;

	if (!show_demo_window)
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	ImGui::ShowDemoWindow(&show_demo_window);
	CustomWindow(&show_equiland_window);
}

#endif // !SOURCE_GUI_HPP 