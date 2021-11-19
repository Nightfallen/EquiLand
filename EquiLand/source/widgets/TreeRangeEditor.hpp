#ifndef SOURCE_WIDGETS_TREERANGEEDITOR_HPP
#define SOURCE_WIDGETS_TREERANGEEDITOR_HPP

#include <includes_pch.h>
#include <source/widgets/BasicCustomWidgets.hpp>
#include <fonts/DefinesFontAwesome.hpp>

namespace widgets::EquiLand {

	struct RangeNode {
		bool isHeader;
		std::string name;
		bool state_context_menu;
		bool state_rename;
		std::vector<std::unique_ptr<RangeNode>>* childs;
	};

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
				.childs = nullptr
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
				.name = "Blank range###",
				.state_context_menu = false,
				.state_rename = false,
				.childs = nullptr
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
			std::copy(node->name.begin(), node->name.end() - 3, buf);

		ImGui::SetNextItemWidth(maxItem.x - minItem.x);
		if (ImGui::InputText("##Rename current Node", buf, 256))
		{
			if (ImGui::IsItemDeactivated())
			{
				node->name = buf;
				//node->name.append("###");
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

	std::string RealRecursiveTreeRange(std::vector<std::unique_ptr<RangeNode>>* nodes, std::string selMask, std::string& real_selected, int rec_level = 1, int id_start = 0)
	{
		auto& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();
		static uint64_t some_id = 1ll << 20;
		ImVec2 curPos = {};
		int result = NodeContextMenuResult_None;
		RangeNode* to_delete_category = nullptr;
		RangeNode* to_delete_range = nullptr;
		bool selected_node = false;

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
					auto result = RealRecursiveTreeRange(node->childs, selected_mask, real_selected, rec_level + 1, (i + 1) * 250);
					if (!result.empty() && result[result.size() - 1] == ';')
					{
						selected_node = true;
						selected_mask = result;
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
						label = child_node->name.data();
						if (child_node->state_rename)
							label = ""; // = "###"

						std::string complete_sel_mask = selected_mask + ">" + std::to_string(j) + ";";
						if (real_selected == complete_sel_mask)
						{
							node_flags |= ImGuiTreeNodeFlags_Selected;
						}

						ImGui::TreeNodeEx((void*)(intptr_t)(id), node_flags, label.data());

						//std::string selected_item = std::format("{}>{}");

						if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
						{
							selected_node = true;
							selected_mask.append(">").append(std::to_string(j)).append(";");
						}

						if (ImGui::BeginPopupContextItem())
						{
							auto result = RangeContextMenu(child_node.get(), *node->childs);
							ImGui::EndPopup();
							if (result == NodeContextMenuResult_Delete)
								to_delete_range = child_node.get();
						}

						if (child_node->state_rename)
							StateRenameChildHelper(child_node.get());

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
			return selected_mask;
		return "";

	}

	void RecursiveTreeRange(std::vector<std::unique_ptr<RangeNode>>* nodes)
	{
		static std::string real_selected;
		std::string sel_mask;
		auto result = RealRecursiveTreeRange(nodes, sel_mask, real_selected, 1, 0);
		if (!result.empty() && result[result.size() - 1] == ';')
			real_selected = result;
	}

	void TreeRangeEditor()
	{
		auto& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();

		static std::vector<std::unique_ptr<RangeNode>> nodes;

		// Buttons to add/delete headers
		const char* edit_buttons[] = { ICON_FA_PLUS_CIRCLE, ICON_FA_FILE, ICON_FA_LIST };
		size_t sz_Arr = ARRAYSIZE(edit_buttons);

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
					.childs = nullptr
					});
				nodes.push_back(std::move(node));
			}
			separator = true;
		}

		RecursiveTreeRange(&nodes);

		ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_SpanFullWidth;
		if (ImGui::CollapsingHeader("Default Ranges", ImGuiTreeNodeFlags_None))
		{
			int node_clicked = -1;
			const char* leafs[] = { "OR UTG-1", "OR CO", "OR BU" };
			static int selection_mask = (1 << 0);

			for (int i = 0; i < 3; ++i)
			{
				ImGuiTreeNodeFlags node_flags = base_flags;
				const bool is_selected = (selection_mask & (1 << i)) != 0;
				if (is_selected)
					node_flags |= ImGuiTreeNodeFlags_Selected;
				node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

				ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, leafs[i], i);
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					node_clicked = i;
			}

			if (node_clicked != -1)
			{
				// Update selection state
				// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
				if (ImGui::GetIO().KeyCtrl)
					selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
				else if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
					selection_mask = (1 << node_clicked);           // Click to single-select
			}

		}
		if (ImGui::CollapsingHeader("NL2-NL10 Ranges", ImGuiTreeNodeFlags_None))
		{
			constexpr auto nFilters = 6;
			const char* filters[nFilters] = { "Open-Raise", "Call OR", "3-Bet", "Call 3-Bet", "4-Bet", "5-Bet - Push" };

			for (int i = 0; i < nFilters; ++i)
			{
				auto curPos = ImGui::GetCursorScreenPos();
				curPos.x += 20.f;
				ImGui::SetCursorScreenPos(curPos);
				if (ImGui::CollapsingHeader(filters[i], ImGuiTreeNodeFlags_None))
				{
					int node_clicked = -1;
					const char* leafs[] = { "OR UTG-1", "OR CO", "OR BU", "OR UTG-1", "OR CO", "OR BU", "OR UTG-1", "OR CO", "OR BU" };
					size_t szLeafs = ARRAYSIZE(leafs);
					static int selection_mask = (1 << 0);

					for (int i = 0; i < szLeafs; ++i)
					{
						ImGuiTreeNodeFlags node_flags = base_flags;
						const bool is_selected = (selection_mask & (1 << i)) != 0;
						if (is_selected)
							node_flags |= ImGuiTreeNodeFlags_Selected;
						node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;



						ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, leafs[i], i);
						if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
							node_clicked = i;
					}

					if (node_clicked != -1)
					{
						// Update selection state
						// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
						if (ImGui::GetIO().KeyCtrl)
							selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
						else if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
							selection_mask = (1 << node_clicked);           // Click to single-select
					}
				}
			}
		}
		if (ImGui::CollapsingHeader("NL10-NL25 Ranges", ImGuiTreeNodeFlags_None))
		{
			constexpr auto nSzArr = 3;
			static bool selection[nSzArr] = { false, false, false };
			const char* leafs[nSzArr] = { "UTG-1 3-Bet", "BB 3-Bet", "SB 3-Bet" };

			for (int n = 0; n < nSzArr; n++)
			{
				if (ImGui::Selectable(leafs[n], selection[n]))
				{
					//if (!ImGui::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
					memset(selection, 0, sizeof(selection));
					selection[n] ^= 1;
				}
			}
		}
		if (ImGui::CollapsingHeader("NL25-100 Ranges", ImGuiTreeNodeFlags_None))
		{
			ImGui::Text("IsItemHovered: %d", ImGui::IsItemHovered());
			for (int i = 0; i < 5; i++)
				ImGui::Text("Some content %d", i);
		}
		if (ImGui::CollapsingHeader("MTT Ranges (ABI 1)", ImGuiTreeNodeFlags_None))
		{
			ImGui::Text("IsItemHovered: %d", ImGui::IsItemHovered());
			for (int i = 0; i < 5; i++)
				ImGui::Text("Some content %d", i);
		}
		if (ImGui::CollapsingHeader("MTT Ranges (ABI 10)", ImGuiTreeNodeFlags_None))
		{
			ImGui::Text("IsItemHovered: %d", ImGui::IsItemHovered());
			for (int i = 0; i < 5; i++)
				ImGui::Text("Some content %d", i);
		}
		if (ImGui::CollapsingHeader("MTT Ranges (ABI 50)", ImGuiTreeNodeFlags_None))
		{
			ImGui::Text("IsItemHovered: %d", ImGui::IsItemHovered());
			for (int i = 0; i < 5; i++)
				ImGui::Text("Some content %d", i);
		}

	}
}

#endif // !SOURCE_WIDGETS_TREERANGEEDITOR_HPP