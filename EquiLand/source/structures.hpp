#ifndef SOURCE_STRUCTURES_HPP
#define SOURCE_STRUCTURES_HPP

typedef void (*ProcUIHandler)();

struct RangeNode {
	bool isHeader;
	std::string name;
	bool state_context_menu;
	bool state_rename;
	std::vector<std::unique_ptr<RangeNode>>* childs;
	std::bitset<169> selectables;
};


#endif // !SOURCE_STRUCTURES_HPP