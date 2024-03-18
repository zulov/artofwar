#pragma once
#include <algorithm>
#include <vector>

class ComplexBucketData;
constexpr char CLOSE_SIZE = 9;
constexpr char FROM_1_TO_2_SIZE = 16;
constexpr char CLOSE_SECOND_SIZE = 25;

class CloseIndexes {
public:
	explicit CloseIndexes(short res);
	CloseIndexes(const CloseIndexes& rhs) = delete;

	const std::vector<short>& getLv1(int center) const { return closeVals[getIndex(center)]; }
	const std::vector<short>& getLv1(const ComplexBucketData& data) const ;
	const std::vector<short>& getLv2(int center) const { return closeSecondVals[getSecondIndex(center)]; }
	const std::vector<short>& getLv2(const ComplexBucketData& data) const;

	const std::vector<unsigned char>& getTabIndexes(const ComplexBucketData& data);
	const std::vector<std::pair<unsigned char, short>>& getTabIndexesWithValue(const ComplexBucketData& data) const;

	const std::vector<std::pair<unsigned char, short>>& getTabIndexesWithValueFreeOnly(const ComplexBucketData& data) const;

	unsigned short getResolution() const { return resolution; }
	bool isInLocalArea(int center, int indexOfAim) const;
	bool isInLocalLv2Area(int center, int indexOfAim) const;
	const std::vector<short>& getPassIndexVia1LevelTo2(int startIdx, int endIdx) const;

	std::pair<unsigned char,unsigned char> getBothIndexes(int center) const;
private:
	unsigned char getIndex(int center) const;
	unsigned char getSecondIndex(int center) const;
	bool validateCloseIndexes(ComplexBucketData const& data) const;

	const std::vector< std::pair<unsigned char, short>>& getTabIndexesWithValueLv2(int center) const { return tabSecondIndexesWithValue[getSecondIndex(center)]; }

	short resolution;
	int sqResolutionMinusRes;
	short templateVec[8];
	short templateVecSecond[16];

	static const std::vector<unsigned char> tabIndexes[CLOSE_SIZE];
	std::vector<short> closeVals[CLOSE_SIZE];
	std::vector<std::pair<unsigned char, short>> tabIndexesWithValue[CLOSE_SIZE];

	static const std::vector<unsigned char> tabSecondIndexes[CLOSE_SECOND_SIZE];
	std::vector<short> closeSecondVals[CLOSE_SECOND_SIZE];
	std::vector<std::pair<unsigned char, short>> tabSecondIndexesWithValue[CLOSE_SECOND_SIZE];

	static const std::vector<unsigned char> passTo2From1Indexes[FROM_1_TO_2_SIZE];
	std::vector<short> passTo2From1Vals[FROM_1_TO_2_SIZE];
	static const std::vector<short> EMPTY;

	std::vector<std::pair<unsigned char, short>> tabIndexesWithValueOnlyFree[256];
};

inline bool isInTab(const std::vector<short>& tab, const int val) {
	return std::ranges::any_of(tab, [val](short i) { return val == i; });
}
