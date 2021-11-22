#ifndef SOURCE_APPSETTINGS_HPP
#define SOURCE_APPSETTINGS_HPP

#include <includes_pch.h>
#include <source/structures.hpp>

namespace {
	const char* path = "";
	const char* app_folder = "EquiLand";

	const char* file_ranges = "ranges.data";
	const char* file_preferences = "settings.cfg";
	const char* file_tasks = "tasks.json";
}

std::string GetAppFolderName() { return app_folder; }

auto GetAppDataFolder() -> std::filesystem::path
{
#if defined(_WIN_BUILD)
	auto temp_path = std::filesystem::temp_directory_path();
	temp_path.parent_path().parent_path();
	temp_path /= app_folder;
#elif defined(_LINUX_BUILD)
	auto temp_path = std::filesystem::path("$HOME/." + std::string(app_folder));
#endif

	return temp_path;
}

nlohmann::json RecursiveRangeSaving(std::vector<std::unique_ptr<RangeNode>>& vec)
{
	using namespace nlohmann;
	json j_arr = json::array();
	std::string format_str =
		"{}{{\n"
		"Name: {}\n"
		"IsHeader: {}\n"
		"Childs: {}\n"
		"}}";
	
	const char* separator = "";
	std::string result;
	std::string formatted;
	for (auto&& node : vec)
	{
		json j;
		j["Name"] = node->name;
		j["IsHeader"] = node->isHeader;
		if (node->childs)
			j["Childs"] = RecursiveRangeSaving(*node->childs);
		else
			j["Childs"] = 0;
		j["Selectables"] = node->selectables.to_string();
		j_arr.push_back(j);
	}

	return j_arr;
}

//auto ReadRecursiveChilds(nlohmann::json j) -> std::vector<std::unique_ptr<RangeNode>>*
//{
//	std::vector<std::unique_ptr<RangeNode>>* result = new std::vector<std::unique_ptr<RangeNode>>();
//
//	
//}

auto ReadFirstArray(nlohmann::json j) -> std::vector<std::unique_ptr<RangeNode>>*
{
	std::vector<std::unique_ptr<RangeNode>>* result = new std::vector<std::unique_ptr<RangeNode>>();
	//for (auto it = j.begin(); it != j.end(); ++it)
	for (auto&& element : j)
	{
		bool isHeader = element.at("IsHeader");
		std::string name = element.at("Name");
		std::vector<std::unique_ptr<RangeNode>>* childs = nullptr;
		auto& child_json = element["Childs"];
		std::cout << std::endl << child_json.dump() << std::endl;
		if (child_json.dump() != "0")
			childs = ReadFirstArray(child_json);
		std::string str_sels = element["Selectables"];
		std::bitset<169> selectables{ str_sels };

		result->push_back(std::make_unique<RangeNode>(isHeader, name, false, false, childs, selectables));
	}

	return result;
}

auto ReadRanges(std::string path) -> std::vector<std::unique_ptr<RangeNode>>*
{
	using namespace nlohmann;
	std::ifstream ifs(path);
	std::string result;
	result.reserve(4096);
	std::string buffer;
	buffer.reserve(256);

	if (!ifs.is_open())
		return new std::vector<std::unique_ptr<RangeNode>>();

	while (!ifs.eof())
	{
		std::getline(ifs, buffer);
		result.append(buffer);
	}
	ifs.close();

	json j = json::parse(result.begin(), result.end(), nullptr, false);
	if (j.is_discarded())
		assert(true, "Json is discrarded");
	std::cout << j.dump();
	std::cout << std::endl << std::endl << std::endl;
	return ReadFirstArray(j);
}

void SaveRanges(std::string path, std::vector<std::unique_ptr<RangeNode>>* data)
{
	auto result = RecursiveRangeSaving(*data);
	std::ofstream ofs(path);
	ofs << result;
	ofs.close();
}

void SavePreferences()
{

}

void SaveTasks()
{

}

void SaveSettings()
{
	//SaveRanges();
	//SavePreferences();
	//SaveTasks();
}

void ReadRanges()
{

}

void ReadPreferences()
{

}

void ReadTasks()
{

}

void ReadSettings()
{
	ReadRanges();
	ReadPreferences();
	ReadTasks();
	return; // SomeSettingsStruct
}


#endif // !SOURCE_APPSETTINGS_HPP