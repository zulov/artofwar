#pragma once
#include <algorithm>
#include <cassert>
#include <vector>

constexpr char CLOSE_SIZE = 9;
constexpr char FROM_1_TO_2_SIZE = 16;
constexpr char CLOSE_SECOND_SIZE = 25;

class CloseIndexes {
public:
	explicit CloseIndexes(short res);
	CloseIndexes(const CloseIndexes& rhs) = delete;

	const std::vector<short>& get(int center) const { return closeVals[getIndex(center)]; }
	const std::vector<short>& getByIndex(unsigned char index) const { assert(index >= 0 && index < CLOSE_SIZE); return closeVals[index]; }
	const std::vector<short>& getSecond(int center) const { return closeSecondVals[getSecondIndex(center)]; }
	const std::vector<short>& getSecondByIndex(unsigned char index) const { assert(index >= 0 && index < CLOSE_SECOND_SIZE); return closeSecondVals[index]; }

	const std::vector<unsigned char>& getTabIndexes(int center) const { return tabIndexes[getIndex(center)]; }
	const std::vector<unsigned char>& getTabIndexesByIndex(unsigned char index) const { assert(index >= 0 && index < CLOSE_SIZE); return tabIndexes[index]; }
	const std::vector<std::pair<unsigned char,short>>& getTabIndexesWithValue(int center) const { return tabIndexesWithValue[getIndex(center)]; }
	const std::vector<std::pair<unsigned char,short>>& getTabIndexesWithValueByIndex(unsigned char index) const { assert(index >= 0 && index < CLOSE_SIZE); return tabIndexesWithValue[index]; }

	const std::vector< std::pair<unsigned char, short>>& getTabSecondIndexesWithValue(int center) const { return tabSecondIndexesWithValue[getSecondIndex(center)]; }

	unsigned short getResolution() const { return resolution; }
	bool isInLocalArea(int center, int indexOfAim) const;
	bool isInLocal2Area(int center, int indexOfAim) const;
	const std::vector<short>& getPassIndexVia1LevelTo2(int startIdx, int endIdx) const;
	std::pair<const std::vector<short>&, int> getPassIndexVia1LevelTo2(int startIdx,
	                                                                   const std::vector<int>& endIdxs) const;
	char getIndex(int center) const;
	char getSecondIndex(int center) const;
private:

	short resolution;
	int sqResolutionMinusRes;
	std::vector<short> templateVec;
	std::vector<short> templateVecSecond;

	static const std::vector<unsigned char> tabIndexes[CLOSE_SIZE];
	std::vector<short> closeVals[CLOSE_SIZE];
	std::vector<std::pair<unsigned char, short>> tabIndexesWithValue[CLOSE_SIZE];

	static const std::vector<unsigned char> tabSecondIndexes[CLOSE_SECOND_SIZE];
	std::vector<short> closeSecondVals[CLOSE_SECOND_SIZE];
	std::vector<std::pair<unsigned char, short>> tabSecondIndexesWithValue[CLOSE_SECOND_SIZE];

	static const std::vector<unsigned char> passTo2From1Indexes[FROM_1_TO_2_SIZE];
	std::vector<short> passTo2From1Vals[FROM_1_TO_2_SIZE];
	static const std::vector<short> EMPTY;
};

inline bool isInTab(const std::vector<short>& tab, const int val) {
	return std::ranges::any_of(tab, [val](short i) { return val == i; });
}