#ifndef SOURCE_WIDGETS_TREERANGEEDITOR_HPP
#define SOURCE_WIDGETS_TREERANGEEDITOR_HPP

#include <includes_pch.h>
#include <source/widgets/BasicCustomWidgets.hpp>
#include <fonts/DefinesFontAwesome.hpp>
#include <source/structures.hpp>
#include <source/AppSettings.hpp>

namespace widgets::EquiLand {
	enum NodeContextMenuFlags {
		NodeContextMenuFlags_None = 0,
		NodeContextMenuFlags_NoAppendCategory = 1 << 0,
		NodeContextMenuFlags_NoAppendRange = 1 << 1,
		NodeContextMenuFlags_NoRename = 1 << 2,
		NodeContextMenuFlags_NoDelete = 1 << 3,
	};

	void DeleteNodeWithChilds(std::vector<std::unique_ptr<RangeNode>>* nodes)
	{
		if (nodes != nullptr)
		{
			for (auto&& node : *nodes)
			{
				DeleteNodeWithChilds(node->childs);
			}
			delete nodes;
			nodes = nullptr;
		}
	}

	enum NodeContextMenuResult
	{
		NodeContextMenuResult_None = 0,
		NodeContextMenuResult_AddCategory = 1,
		NodeContextMenuResult_AddRange,
		NodeContextMenuResult_Rename,
		NodeContextMenuResult_Delete,
	};

	inline int NodeContextMenu(RangeNode* node, std::vector<std::unique_ptr<RangeNode>>& nodes, int flags = NodeContextMenuFlags_None)
	{
		bool no_add_category = flags & NodeContextMenuFlags_NoAppendCategory; (flags >> 0) & 1U;
		bool no_add_range = flags & NodeContextMenuFlags_NoAppendRange;
		bool no_rename = flags & NodeContextMenuFlags_NoRename;
		bool no_delete = flags & NodeContextMenuFlags_NoDelete;
		int result = 0;

		if (!no_add_category && ImGui::MenuItemEx("Add Category", ICON_FA_PLUS_CIRCLE))
		{
			using namespace std::string_literals;
			if (node->childs == nullptr)
				node->childs = new std::vector<std::unique_ptr<RangeNode>>();

			auto new_category = std::make_unique<RangeNode>(RangeNode{
				.isHeader = true,
				.name = "Empty Sub Node Header###",
				.state_context_menu = false,
				.state_rename = false,
				.childs = nullptr,
				.selectables = 0b0
				});
			node->childs->push_back(std::move(new_category));
			result = NodeContextMenuResult_AddCategory;
		}
		if (!no_add_range && ImGui::MenuItemEx("Add Range", ICON_FA_PLUS_CIRCLE))
		{
			using namespace std::string_literals;
			if (node->childs == nullptr)
				node->childs = new std::vector<std::unique_ptr<RangeNode>>();

			auto new_range = std::make_unique<RangeNode>(RangeNode{
				.isHeader = false,
				.name = "Blank range",
				.state_context_menu = false,
				.state_rename = false,
				.childs = nullptr,
				.selectables = 0b0
				});
			node->childs->push_back(std::move(new_range));
			result = NodeContextMenuResult_AddRange;
		}
		if (!no_rename && ImGui::MenuItemEx("Rename", ICON_FA_PENCIL))
		{
			node->state_rename = true;
			result = NodeContextMenuResult_Rename;
		}
		if (!no_delete && ImGui::MenuItemEx("Delete", ICON_FA_TRASH_O))
		{
			result = NodeContextMenuResult_Delete;
		}
		return result;
	}

	inline int RangeContextMenu(RangeNode* node, std::vector<std::unique_ptr<RangeNode>>& nodes, int flags = NodeContextMenuFlags_None)
	{
		bool no_add_category = flags & NodeContextMenuFlags_NoAppendCategory; (flags >> 0) & 1U;
		bool no_add_range = flags & NodeContextMenuFlags_NoAppendRange;
		bool no_rename = flags & NodeContextMenuFlags_NoRename;
		bool no_delete = flags & NodeContextMenuFlags_NoDelete;
		int result = 0;

		if (!no_rename && ImGui::MenuItemEx("Rename", ICON_FA_PENCIL))
		{
			node->state_rename = true;
			result = NodeContextMenuResult_Rename;
		}
		if (!no_delete && ImGui::MenuItemEx("Delete", ICON_FA_TRASH_O))
		{
			result = NodeContextMenuResult_Delete;
		}
		return result;
	}

	void StateRenameHelper(RangeNode* node, bool is_header = true)
	{
		auto& style = ImGui::GetStyle();
		auto minItem = ImGui::GetItemRectMin();
		auto maxItem = ImGui::GetItemRectMax();
		auto nodeTextOffset = ImGui::GetTreeNodeToLabelSpacing();
		nodeTextOffset += style.FramePadding.x;


		minItem.x += nodeTextOffset;
		ImGui::SetCursorScreenPos(minItem);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, { 0, 0, 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
		char buf[256]{};

		if (!ImGui::IsAnyItemFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
			std::copy(node->name.begin(), node->name.end() - 3, buf);

		ImGui::SetNextItemWidth(maxItem.x - minItem.x);
		if (ImGui::InputText("##Rename current Node", buf, 256))
		{
			if (ImGui::IsItemDeactivated())
			{
				node->name = buf;
				node->name.append("###");
				node->state_rename = false;
			}
		}

		if (!ImGui::IsAnyItemFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
		{
			ImGui::SetKeyboardFocusHere(-1);
		}
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();
	}

	void StateRenameChildHelper(RangeNode* node)
	{
		auto& style = ImGui::GetStyle();
		auto minItem = ImGui::GetItemRectMin();
		auto maxItem = ImGui::GetItemRectMax();
		auto nodeTextOffset = ImGui::GetTreeNodeToLabelSpacing();
		nodeTextOffset += style.FramePadding.x;
		maxItem.x += ImGui::GetContentRegionAvailWidth() - nodeTextOffset;

		minItem.x += nodeTextOffset;
		ImGui::SetCursorScreenPos(minItem);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, { 0, 0, 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
		char buf[256]{};

		if (!ImGui::IsAnyItemFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
			std::copy(node->name.begin(), node->name.end(), buf);

		ImGui::SetNextItemWidth(maxItem.x - minItem.x);
		if (ImGui::InputText("##Rename current Node", buf, 256))
		{
			if (ImGui::IsItemDeactivated())
			{
				node->name = buf;
				node->state_rename = false;
			}
		}

		if (!ImGui::IsAnyItemFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
		{
			ImGui::SetKeyboardFocusHere(-1);
		}
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();
	}

	struct TreeRangeRes {
		std::string name;
		std::bitset<169>* range;
	};

	TreeRangeRes RealRecursiveTreeRange(std::vector<std::unique_ptr<RangeNode>>* nodes, std::string selMask, std::string& real_selected, int rec_level = 1, int id_start = 0)
	{
		auto& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();
		static uint64_t some_id = 1ll << 20;
		ImVec2 curPos = {};
		int result = NodeContextMenuResult_None;
		RangeNode* to_delete_category = nullptr;
		RangeNode* to_delete_range = nullptr;
		bool selected_node = false;
		TreeRangeRes return_result = {};
		std::bitset<169>* range_result = nullptr;

		auto selected_mask = selMask;
		for (int i = 0; auto && node : *nodes)
		{
			i++;
			if (!node->isHeader) continue;
			ImGui::PushID(some_id + id_start + i);
			if (!selected_node)
				selected_mask = selMask + ">" + std::to_string(i);

			auto label = node->name;
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
			if (node->state_rename)
			{
				label = "###";
				flags |= ImGuiTreeNodeFlags_AllowItemOverlap;
			}

			curPos = ImGui::GetCursorScreenPos();
			curPos.x += ImGui::GetTreeNodeToLabelSpacing() * (rec_level - 1);
			ImGui::SetCursorScreenPos(curPos);
			bool open = ImGui::CollapsingHeader(label.data(), flags);

			if (ImGui::BeginPopupContextItem())
			{
				result = NodeContextMenu(node.get(), *nodes);
				ImGui::EndPopup();
				if (result == NodeContextMenuResult_Delete)
					to_delete_category = node.get();
			}

			if (node->state_rename)
			{
				StateRenameHelper(node.get());
			}

			if (open)
			{
				if (node->childs)
				{
					auto func_temp_result = RealRecursiveTreeRange(node->childs, selected_mask, real_selected, rec_level + 1, (i + 1) * 250);
					auto& str_tmp_res = func_temp_result.name;
					if (!str_tmp_res.empty() && str_tmp_res[str_tmp_res.size() - 1] == ';')
					{
						selected_node = true;
						selected_mask = str_tmp_res;
						range_result = func_temp_result.range;
					}

					// Maybe additional push id is needed
					for (int j = 0; auto && child_node : *node->childs)
					{
						++j;
						if (child_node->isHeader) continue;
						int id = some_id + 1000 + id_start + j;
						ImGui::PushID(id);
						bool selected = true;
						int node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
						
						if (child_node->state_rename)
							node_flags |= ImGuiTreeNodeFlags_AllowItemOverlap;

						label = child_node->name.data();
						if (child_node->state_rename)
							label = ""; // = "###"

						std::string complete_sel_mask = selected_mask + ">" + std::to_string(j) + ";";
						if (real_selected == complete_sel_mask)
						{
							node_flags |= ImGuiTreeNodeFlags_Selected;
						}

						ImGui::TreeNodeEx((void*)(intptr_t)(id), node_flags, label.data());

						if (child_node->state_rename)
							StateRenameChildHelper(child_node.get());

						//std::string selected_item = std::format("{}>{}");

						if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
						{
							selected_node = true;
							selected_mask.append(">").append(std::to_string(j)).append(";");
							range_result = &child_node->selectables;
						}

						if (ImGui::BeginPopupContextItem())
						{
							auto result = RangeContextMenu(child_node.get(), *node->childs);
							ImGui::EndPopup();
							if (result == NodeContextMenuResult_Delete)
								to_delete_range = child_node.get();
						}

						

						ImGui::PopID();
					}

				}
			}
			ImGui::PopID();
		}


		if (to_delete_category)
		{
			auto& name = to_delete_category->name;
			for (int i = 0; auto && node : *nodes)
			{
				if (name == node->name)
				{
					DeleteNodeWithChilds(node->childs);
					nodes->erase(nodes->begin() + i);
					break;
				}
				i++;
			}
		}

		if (to_delete_range)
		{
			auto& name = to_delete_range->name;
			for (auto&& node : *nodes)
			{
				for (int i = 0; auto && child_node : *node->childs)
				{
					if (name == child_node->name)
					{
						node->childs->erase(node->childs->begin() + i);
						break;
					}
					++i;
				}
			}
		}

		if (selected_node)
			return { selected_mask, range_result };
		return { "", range_result };

	}

	std::bitset<169>* RecursiveTreeRange(std::vector<std::unique_ptr<RangeNode>>* nodes)
	{
		static std::string real_selected;
		std::string sel_mask;
		auto result_full = RealRecursiveTreeRange(nodes, sel_mask, real_selected, 1, 0);
		auto& result = result_full.name;
		if (!result.empty() && result[result.size() - 1] == ';')
			real_selected = result;
		return result_full.range;
	}

	std::bitset<169>* TreeRangeEditor()
	{
		auto& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();
		
		static bool only_once = true;
		auto custom_deleter = [](std::vector<std::unique_ptr<RangeNode>>* nodes) { delete nodes; };
		static std::unique_ptr<std::vector<std::unique_ptr<RangeNode>>> nodes(ReadRanges("ranges.txt"));



		if (only_once)
		{
			auto nodes_from_file = ReadRanges("ranges.txt");
			//nodes = std::move(std::make_unique<std::vector<std::unique_ptr<RangeNode>>>(nodes_from_file));
			only_once = false;
		}

		// Buttons to add/delete headers
		const char* edit_buttons[] = { ICON_FA_PLUS_CIRCLE, ICON_FA_FILE, ICON_FA_LIST };
		size_t sz_Arr = IM_ARRAYSIZE(edit_buttons);

		bool separator = false;
		for (int i = 0; i < sz_Arr; ++i)
		{
			if (separator) ImGui::SameLine();
			if (ImGui::Button(edit_buttons[i]))
			{
				std::string name_node = "Empty Node Header###";
				auto node = std::make_unique<RangeNode>(RangeNode{
					.isHeader = true,
					.name = name_node,
					.state_context_menu = false,
					.state_rename = false,
					.childs = nullptr,
					.selectables = 0b0
					});
				nodes->push_back(std::move(node));
			}
			separator = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Save ranges"))
		{
			SaveRanges("ranges.txt", nodes.get());
		}
		// Auto saving on application exit
		// This works due fade-out animation on app exit
		// #FIXME Temp hack
		if (style.Alpha <= 0.2f)
			SaveRanges("ranges.txt", nodes.get());

		auto range_res =  RecursiveTreeRange(nodes.get());
		return range_res;
	}
}

#endif // !SOURCE_WIDGETS_TREERANGEEDITOR_HPP