#include "pch.h"

#include <regex>
#include <vector>

#include "utils/StringUtils.h"
#include "utils/replace_utils.h"

class StringAndRegexUtilsFixture : public ::testing::Test {};

TEST_F(StringAndRegexUtilsFixture, SplitSeparatesDelimitedValues) {
	auto tokens = split("alpha,beta,gamma", ',');

	EXPECT_EQ(tokens, std::vector<std::string>({ "alpha", "beta", "gamma" }));
}

TEST_F(StringAndRegexUtilsFixture, SplitPreservesEmptyFieldsBetweenDelimiters) {
	auto tokens = split("a,,c", ',');

	EXPECT_EQ(tokens, std::vector<std::string>({ "a", "", "c" }));
}

TEST_F(StringAndRegexUtilsFixture, SplitReturnsNoTokensForEmptyString) {
	auto tokens = split("", ',');

	EXPECT_TRUE(tokens.empty());
}

TEST_F(StringAndRegexUtilsFixture, SplitDropsTrailingEmptyFieldLikeStdGetline) {
	auto tokens = split("a,", ',');

	EXPECT_EQ(tokens, std::vector<std::string>({ "a" }));
}

TEST_F(StringAndRegexUtilsFixture, AsStringFUsesRequestedPrecision) {
	EXPECT_EQ(asStringF(3.14159f), "3.14");
	EXPECT_EQ(asStringF(3.14159f, 4), "3.1416");
}

TEST_F(StringAndRegexUtilsFixture, To2DigStringPadsSingleDigitValues) {
	EXPECT_STREQ(to2DigString(0).CString(), "00");
	EXPECT_STREQ(to2DigString(5).CString(), "05");
	EXPECT_STREQ(to2DigString(12).CString(), "12");
}

TEST_F(StringAndRegexUtilsFixture, RegexReplaceSubstitutesSequentialValues) {
	std::vector<int> values = { 10, 20 };
	std::regex regex("\\?");

	EXPECT_EQ(std::regex_replace(std::string("A?B?C"), regex, values), "A10B20C");
}

TEST_F(StringAndRegexUtilsFixture, RegexReplacePreservesUnmatchedTextAndAdjacentMatches) {
	std::vector<int> values = { 7, 8, 9 };
	std::regex regex("#");

	EXPECT_EQ(std::regex_replace(std::string("#x##"), regex, values), "7x89");
}

TEST_F(StringAndRegexUtilsFixture, RegexReplaceLeavesStringUntouchedWhenNoMatchesExist) {
	std::vector<int> values = { 1, 2 };
	std::regex regex("#");

	EXPECT_EQ(std::regex_replace(std::string("abc"), regex, values), "abc");
}
