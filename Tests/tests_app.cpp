#include <gtest/gtest.h>
#include <string>
//#include <Settings.hpp>

#include "../EquiLand/source/Settings.hpp"

TEST(SettingsParsing, RegexTests) {
	using namespace std::string_literals;

	EXPECT_EQ(GetSomeSetting(), "some_value"s);
}