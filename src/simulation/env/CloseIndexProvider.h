#pragma once
#include <vector>

constexpr char CLOSE_SIZE = 9;
constexpr char CLOSE_SECOND_SIZE = 25;

class CloseIndexProvider {
public:
	explicit CloseIndexProvider(short res);
	CloseIndexProvider(const CloseIndexProvider& rhs) = delete;
	const std::vector<short>& get(int center) const;
	const std::vector<short>& getSecond(int center) const;
	const std::vector<char>& getTabIndexes(int center) const;
	short getIndexAt(char index) const;

private:
	char getIndex(int center) const;

	short resolution;
	std::vector<short> templateVec;
	std::vector<short> templateVecSecond;

	std::vector<char> tabIndexes[CLOSE_SIZE];
	std::vector<short> closeIndexes[CLOSE_SIZE];

	std::vector<char> tabIndexesSecond[CLOSE_SECOND_SIZE];
	std::vector<short> closeIndexesSecond[CLOSE_SECOND_SIZE];
};
