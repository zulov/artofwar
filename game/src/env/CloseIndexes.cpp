#include <vector>

#include "CloseIndexes.h"

#include <cassert>

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
	{7, 9, 11, 12, 13, 14, 15},
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
	  templateVec{-res - 1, -res, -res + 1, -1, 1, res - 1, res, res + 1},
	  templateVecSecond{
		  -2 * res - 2, -2 * res - 1, -2 * res, -2 * res + 1, -2 * res + 2,
		  -res - 2, -res + 2,
		  -2, +2,
		  res - 2, res + 2,
		  2 * res - 2, 2 * res - 1, 2 * res, 2 * res + 1, 2 * res + 2,
	  } {
	for (int i = 0; i < CLOSE_SIZE; ++i) {
		closeVals[i].reserve(tabIndexes[i].size());
		tabIndexesWithValue[i].reserve(tabIndexes[i].size());
		for (auto j : tabIndexes[i]) {
			closeVals[i].emplace_back(templateVec[j]);
			tabIndexesWithValue[i].emplace_back(std::pair(j, templateVec[j]));
		}
	}

	for (int i = 0; i < CLOSE_SECOND_SIZE; ++i) {
		closeSecondVals[i].reserve(tabSecondIndexes[i].size());
		for (auto j : tabSecondIndexes[i]) {
			closeSecondVals[i].emplace_back(templateVecSecond[j]);
		}
	}

	for (int i = 0; i < FROM_1_TO_2_SIZE; ++i) {
		passTo2From1Vals[i].reserve(passTo2From1Indexes[i].size());
		for (auto j : passTo2From1Indexes[i]) {
			passTo2From1Vals[i].emplace_back(templateVec[j]);
		}
	}
}

bool CloseIndexes::isInLocalArea(const int center, int indexOfAim) const {
	if (center == indexOfAim) { return true; }
	return isInTab(get(indexOfAim), indexOfAim - center); //center + value == indexOfAim
}

bool CloseIndexes::isInLocal2Area(int center, int indexOfAim) const {
	return isInTab(getSecond(indexOfAim), indexOfAim - center); //center + value == indexOfAim
}

const std::vector<short>& CloseIndexes::getPassIndexVia1LevelTo2(int startIdx, int endIdx) const {
	auto& tab = getTabSecondIndexes(endIdx);

	const auto diff = endIdx - startIdx; //startIdx + tab[i] == endIdx
	for (auto i : tab) {
		auto val = getSecondIndexAt(i);
		if (val == diff) {
			return passTo2From1Vals[i];
		}
	}
	assert(!isInLocal2Area(startIdx, endIdx));
	return EMPTY;
}

std::pair<const std::vector<short>&, int> CloseIndexes::getPassIndexVia1LevelTo2(
	int startIdx, const std::vector<int>& endIdxs) const {
	for (auto endIdx : endIdxs) {
		auto& tab = getTabSecondIndexes(endIdx);

		const auto diff = endIdx - startIdx; //startIdx + tab[i] == endIdx
		for (auto i : tab) {
			auto val = getSecondIndexAt(i);
			if (val == diff) {
				return {passTo2From1Vals[i], endIdx};
			}
		}
	}

	return {EMPTY, -1};
}

char CloseIndexes::getIndex(int center) const {
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
	return index;
	// const auto mod = center % resolution;
	//
	// auto a = center < resolution;
	// auto b = center >= sqResolutionMinusRes;
	// auto a1 = mod == 0;
	// auto b1 = mod == resolution - 1;
	// return  !a * (b * 6 + !b * 3) +
	// 	!a1 * (b1 * 2 + !b1 * 1);
}

char CloseIndexes::getSecondIndex(int center) const {
	const bool firstRow = center < resolution;
	const bool secondRow = !firstRow && center < 2 * resolution;

	const bool lastRow = center >= resolution * resolution - resolution;
	const bool almostLastRow = !lastRow && center >= resolution * resolution - 2 * resolution;

	const bool firstColumn = center % resolution == 0;
	const bool secondColumn = center % resolution == 1;

	const bool lastColumn = center % resolution == resolution - 1;
	const bool almostLastColumn = center % resolution == resolution - 2;
	char index = 0;
	if (firstRow) {} else if (secondRow) {
		index += 5;
	} else if (almostLastRow) {
		index += 15;
	} else if (lastRow) {
		index += 20;
	} else {
		index += 10;
	}

	if (firstColumn) {} else if (secondColumn) {
		index += 1;
	} else if (almostLastColumn) {
		index += 3;
	} else if (lastColumn) {
		index += 4;
	} else { index += 2; }
	return index;
}
