#include <gtest/gtest.h>

#include <source/AppSettings.hpp>

TEST(SettingsParsing, RegexTests) {
	using namespace std::string_literals;
	EXPECT_EQ("some_value", "some_value"s);
}

TEST(SettingsParsing, PathFindingTests) {
	EXPECT_EQ(GetAppFolderName(), "EquiLand");
}
