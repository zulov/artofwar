#include "CloseIndexes.h"

const std::vector<char> CloseIndexes::tabIndexes[CLOSE_SIZE] = {
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

const std::vector<char> CloseIndexes::tabIndexesSecond[CLOSE_SECOND_SIZE] = {
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

CloseIndexes::CloseIndexes(short res)
	: resolution(res), templateVec{-res - 1, -res, -res + 1, -1, 1, res - 1, res, res + 1},
	  templateVecSecond{
		  -2 * res - 2, -2 * res - 1, -2 * res, -2 * res + 1, -2 * res + 2,
		  -res - 2, -res + 2,
		  - 2, +2,
		  res - 2, res + 2,
		  2 * res - 2, 2 * res - 1, 2 * res, 2 * res + 1, 2 * res + 2,
	  } {

	for (int i = 0; i < CLOSE_SIZE; ++i) {
		for (auto j : tabIndexes[i]) {
			closeIndexes[i].emplace_back(templateVec[j]);
		}
		closeIndexes[i].shrink_to_fit();
	}

	for (int i = 0; i < CLOSE_SECOND_SIZE; ++i) {
		for (auto j : tabIndexesSecond[i]) {
			closeIndexesSecond[i].emplace_back(templateVecSecond[j]);
		}
		closeIndexesSecond[i].shrink_to_fit();
	}
}

char CloseIndexes::getIndex(int center) const {
	const bool firstRow = center < resolution;
	const bool lastRow = center > resolution * resolution - resolution;
	const bool firstColumn = center % resolution == 0;
	const bool lastColumn = center % resolution == resolution - 1;

	char index = 0;
	if (firstRow) {
	} else if (lastRow) {
		index += 6;
	} else {
		index += 3;
	}
	if (firstColumn) {
	} else if (lastColumn) {
		index += 2;
	} else {
		index += 1;
	}
	return index;
}

const std::vector<short>& CloseIndexes::get(int center) const {
	return closeIndexes[getIndex(center)];
}

const std::vector<short>& CloseIndexes::getSecond(int center) const {
	const bool firstRow = center < resolution;
	const bool secondRow = !firstRow && center < 2 * resolution;

	const bool lastRow = center > resolution * resolution - resolution;
	const bool almostLastRow = !lastRow && center > resolution * resolution - 2 * resolution;

	const bool firstColumn = center % resolution == 0;
	const bool secondColumn = center % resolution == 1;

	const bool lastColumn = center % resolution == resolution - 1;
	const bool almostLastColumn = center % resolution == resolution - 2;
	char index = 0;
	if (firstRow) {
	} else if (secondRow) {
		index += 5;
	} else if (almostLastRow) {
		index += 15;
	} else if (lastRow) {
		index += 20;
	} else {
		index += 10;
	}

	if (firstColumn) {
	} else if (secondColumn) {
		index += 1;
	} else if (almostLastColumn) {
		index += 3;
	} else if (lastColumn) {
		index += 4;
	} else { index += 2; }


	return closeIndexesSecond[index];
}

const std::vector<char>& CloseIndexes::getTabIndexes(int center) const {
	return tabIndexes[getIndex(center)];
}

short CloseIndexes::getIndexAt(char index) const {
	return templateVec[index];
}
