#ifndef SOURCE_APPSETTINGS_HPP
#define SOURCE_APPSETTINGS_HPP

#include <includes_pch.h>

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
	std::filesystem::path temp_path;
#if defined(_WIN_BUILD)
	temp_path = std::filesystem::temp_directory_path();
	temp_path.parent_path().parent_path();
	temp_path /= app_folder;
#elif defined(_LINUX_BUILD)
	auto temp_path = std::filesystem::path("$HOME/." + std::string(app_folder));
#endif

	return temp_path;
}

void SaveRanges()
{

}

void SavePreferences()
{

}

void SaveTasks()
{

}

void SaveSettings()
{
	SaveRanges();
	SavePreferences();
	SaveTasks();
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