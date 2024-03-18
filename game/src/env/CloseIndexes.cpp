#include <vector>
#include <cassert>

#include "CloseIndexes.h"
#include "bucket/ComplexBucketData.h"


const std::vector<unsigned char> CloseIndexes::tabIndexes[CLOSE_SIZE] = {
	{4, 6, 7},
	{3, 4, 5, 6, 7},
	{3, 5, 6},
	{1, 2, 4, 6, 7},
	{0, 1, 2, 3, 4, 5, 6, 7},
	{0, 1, 3, 5, 6},
	{1, 2, 4},
	{0, 1, 2, 3, 4},
	{0, 1, 3}
};

const std::vector<unsigned char> CloseIndexes::tabSecondIndexes[CLOSE_SECOND_SIZE] = {
	{8, 10, 13, 14, 15},
	{8, 10, 12, 13, 14, 15},
	{7, 8, 9, 10, 11, 12, 13, 14, 15},
	{7, 9, 11, 12, 13, 14},
	{7, 9, 11, 12, 13},

	{6, 8, 10, 13, 14, 15},
	{6, 8, 10, 12, 13, 14, 15},
	{5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
	{5, 7, 9, 11, 12, 13, 14},
	{5, 7, 9, 11, 12, 13},

	{2, 3, 4, 6, 8, 10, 13, 14, 15},
	{1, 2, 3, 4, 6, 8, 10, 12, 13, 14, 15},
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
	{0, 1, 2, 3, 5, 7, 9, 11, 12, 13, 14},
	{0, 1, 2, 5, 7, 9, 11, 12, 13},

	{2, 3, 4, 6, 8, 10},
	{1, 2, 3, 4, 6, 8, 10},
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
	{0, 1, 2, 3, 5, 7, 9},
	{0, 1, 2, 5, 7, 9},

	{2, 3, 4, 6, 8},
	{1, 2, 3, 4, 6, 8},
	{0, 1, 2, 3, 4, 5, 6, 7, 8},
	{0, 1, 2, 3, 5, 7},
	{0, 1, 2, 5, 7},
};

const std::vector<unsigned char> CloseIndexes::passTo2From1Indexes[FROM_1_TO_2_SIZE] = {
	{0},
	{0, 1},
	{1, 0, 2},
	{1, 2},
	{2},

	{0, 3},
	{2, 4},
	{3, 0, 5},
	{4, 2, 7},
	{3, 5},
	{4, 7},

	{5},
	{5, 6},
	{6, 5, 7},
	{6, 7},
	{7}
};
const std::vector<short> CloseIndexes::EMPTY = {};

CloseIndexes::CloseIndexes(short res)
	: resolution(res), sqResolutionMinusRes(res * res - res),
	  templateVec{short(-res - 1), short(-res), short(-res + 1), -1, 1, short(res - 1), res, short(res + 1)},
	  templateVecSecond{
		  short(-2 * res - 2), short(-2 * res - 1), short(-2 * res), short(-2 * res + 1), short(-2 * res + 2),
		  short(-res - 2), short(-res + 2),
		  -2, 2,
		  short(res - 2), short(res + 2),
		  short(2 * res - 2), short(2 * res - 1), short(2 * res), short(2 * res + 1), short(2 * res + 2),
	  } {
	for (int i = 0; i < CLOSE_SIZE; ++i) {
		closeVals[i].reserve(tabIndexes[i].size());
		tabIndexesWithValue[i].reserve(tabIndexes[i].size());
		for (auto j : tabIndexes[i]) {
			closeVals[i].emplace_back(templateVec[j]);
			tabIndexesWithValue[i].emplace_back(j, templateVec[j]);
		}
	}

	for (int i = 0; i < CLOSE_SECOND_SIZE; ++i) {
		closeSecondVals[i].reserve(tabSecondIndexes[i].size());
		tabSecondIndexesWithValue[i].reserve(tabSecondIndexes[i].size());
		for (auto j : tabSecondIndexes[i]) {
			closeSecondVals[i].emplace_back(templateVecSecond[j]);
			tabSecondIndexesWithValue[i].emplace_back(j, templateVecSecond[j]);
		}
	}

	for (int i = 0; i < FROM_1_TO_2_SIZE; ++i) {
		passTo2From1Vals[i].reserve(passTo2From1Indexes[i].size());
		for (const auto j : passTo2From1Indexes[i]) {
			passTo2From1Vals[i].emplace_back(templateVec[j]);
		}
	}

	for (int i = 0; i < 256; ++i) {
		const unsigned char val = i;
		auto& vector = tabIndexesWithValueOnlyFree[val];
		for (int b = 0; b < 8; ++b) {
			if (!(val & Flags::bitFlags[b])) {
				vector.push_back(tabIndexesWithValue[4][b]);
			}
		}
	}
}

const std::vector<short>& CloseIndexes::getLv2(const ComplexBucketData& data) const {
	return closeSecondVals[data.getIndexSecondOfCloseIndexes()];
}

const std::vector<unsigned char>& CloseIndexes::getTabIndexes(const ComplexBucketData& data) {
	return tabIndexes[data.getIndexOfCloseIndexes()];
}

const std::vector<std::pair<unsigned char, short>>& CloseIndexes::getTabIndexesWithValue(
	const ComplexBucketData& data) const {
	return tabIndexesWithValue[data.getIndexOfCloseIndexes()];
}

const std::vector<std::pair<unsigned char, short>>& CloseIndexes::getTabIndexesWithValueFreeOnly(
	const ComplexBucketData& data) const {
	assert(validateCloseIndexes(data));
	return tabIndexesWithValueOnlyFree[data.getNeightOccupation()];
}

const std::vector<short>& CloseIndexes::getLv1(const ComplexBucketData& data) const {
	return closeVals[data.getIndexOfCloseIndexes()];
}

bool CloseIndexes::isInLocalArea(const int center, int indexOfAim) const {
	if (center == indexOfAim) { return true; }
	return isInTab(getLv1(indexOfAim), indexOfAim - center); //center + value == indexOfAim
}

bool CloseIndexes::isInLocalLv2Area(int center, int indexOfAim) const {
	return isInTab(getLv2(indexOfAim), indexOfAim - center); //center + value == indexOfAim
}

const std::vector<short>& CloseIndexes::getPassIndexVia1LevelTo2(int startIdx, int endIdx) const {
	const auto diff = endIdx - startIdx; //startIdx + tab[i] == endIdx
	for (auto [i, val] : getTabIndexesWithValueLv2(endIdx)) {
		if (val == diff) {
			return passTo2From1Vals[i];
		}
	}
	assert(!isInLocalLv2Area(startIdx, endIdx));
	return EMPTY;
}

std::pair<unsigned char, unsigned char> CloseIndexes::getBothIndexes(int center) const {
	const bool firstRow = center < resolution;
	const bool secondRow = !firstRow && center < 2 * resolution;

	const bool lastRow = center >= sqResolutionMinusRes;
	const bool almostLastRow = !lastRow && center >= sqResolutionMinusRes - resolution;

	const bool firstColumn = center % resolution == 0;
	const bool secondColumn = center % resolution == 1;

	const bool lastColumn = center % resolution == resolution - 1;
	const bool almostLastColumn = center % resolution == resolution - 2;
	char indexLv2 = 0;
	char indexLv1 = 0;
	if (firstRow) {} else if (secondRow) {
		indexLv2 += 5;
		indexLv1 += 3;
	} else if (almostLastRow) {
		indexLv2 += 15;
		indexLv1 += 3;
	} else if (lastRow) {
		indexLv2 += 20;
		indexLv1 += 6;
	} else {
		indexLv2 += 10;
		indexLv1 += 3;
	}

	if (firstColumn) {} else if (secondColumn) {
		indexLv2 += 1;
		indexLv1 += 1;
	} else if (almostLastColumn) {
		indexLv2 += 3;
		indexLv1 += 1;
	} else if (lastColumn) {
		indexLv2 += 4;
		indexLv1 += 2;
	} else {
		indexLv2 += 2;
		indexLv1 += 1;
	}

	return {indexLv1, indexLv2};
}

unsigned char CloseIndexes::getIndex(int center) const {
	char index = 0;
	if (center < resolution) { } else if (center >= sqResolutionMinusRes) {
		index += 6;
	} else {
		index += 3;
	}
	const auto mod = center % resolution;

	if (mod == 0) { } else if (mod == resolution - 1) {
		index += 2;
	} else {
		index += 1;
	}
	assert(index >= 0 && index < CLOSE_SIZE);
	return index;
}

unsigned char CloseIndexes::getSecondIndex(int center) const {
	return getBothIndexes(center).second;
}

bool CloseIndexes::validateCloseIndexes(ComplexBucketData const& data) const {
	int j = 0;
	//auto& vec = getTabIndexesWithValueFreeOnly(data);// recursion
	auto& vec = tabIndexesWithValueOnlyFree[data.getNeightOccupation()];

	for (auto [i, val] : getTabIndexesWithValue(data)) {
		if (data.ifNeightIsFree(i)) {
			if (vec[j].first != i || vec[j].second != val) {
				return false;
			}
			++j;
		}
	}
	return j == vec.size();
}
