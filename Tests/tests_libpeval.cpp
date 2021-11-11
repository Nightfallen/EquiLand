#include <gtest/gtest.h>
#include <helpers.hpp>

TEST(HELPERS, nChoosek) {
	using peval::nChoosek;
	EXPECT_EQ(1326,			nChoosek(52,2));
	EXPECT_EQ(270725,		nChoosek(52, 4));
	EXPECT_EQ(2598960,		nChoosek(52, 5));
	EXPECT_EQ(20358520,		nChoosek(52, 6));
	EXPECT_EQ(133784560,	nChoosek(52, 7));
	EXPECT_EQ(499500,		nChoosek(1000, 2));

	EXPECT_EQ(630,			nChoosek(36, 2));
	EXPECT_EQ(58905,		nChoosek(36, 4));
	EXPECT_EQ(376992,		nChoosek(36, 5));
}