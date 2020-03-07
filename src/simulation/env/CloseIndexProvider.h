#pragma once
#include <vector>

class CloseIndexProvider {
public:
	explicit CloseIndexProvider(short res);
	const std::vector<short>& get(int center) const;
	const std::vector<short>& getSecond(int center) const;
	const std::vector<char>& getTabIndexes(int center) const;
	short getIndexAt(char index) const;

private:
	char getIndex(int center) const;

	short resolution;
	std::vector<short> templateVec;

	std::vector<char> tabIndexes[9];
	std::vector<short> closeIndexes[9];

	std::vector<char> tabIndexesSecond[25];
	std::vector<short> closeIndexesSecond[25];
};
