#include "CloseIndexProvider.h"

CloseIndexProvider::CloseIndexProvider(short res)
	: resolution(res),
	  templateVec{-res - 1, -res, -res + 1, -1, 1, res - 1, res, res + 1},
	  templateVecSecond{
		  -2 * res - 2, -2 * res - 1, -2 * res, -2 * res + 1, -2 * res + 2,
		  -res - 2, -res + 2,
		  - 2, +2,
		  res - 2, res + 2,
		  2 * res - 2, 2 * res - 1, 2 * res, 2 * res + 1, 2 * res + 2,
	  },
	  tabIndexes{
		  {4, 6, 7},
		  {3, 4, 5, 6, 7},
		  {3, 5, 6},
		  {1, 2, 4, 6, 7},
		  {0, 1, 2, 3, 4, 5, 6, 7},
		  {0, 1, 3, 5, 6},
		  {1, 2, 4},
		  {0, 1, 2, 3, 4},
		  {0, 1, 3}
	  }, tabIndexesSecond{
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
	int a = 5;
}

char CloseIndexProvider::getIndex(int center) const {
	bool firstRow = center < resolution;
	bool lastRow = center > resolution * resolution - resolution;
	bool firstColumn = center % resolution == 0;
	bool lastColumn = center % resolution == resolution - 1;

	char index = 0;
	if (firstRow) { } else if (lastRow) {
		index += 6;
	} else {
		index += 3;
	}
	if (firstColumn) { } else if (lastColumn) {
		index += 2;
	} else {
		index += 1;
	}
	return index;
}

const std::vector<short>& CloseIndexProvider::get(int center) const {
	return closeIndexes[getIndex(center)];
}

const std::vector<short>& CloseIndexProvider::getSecond(int center) const {
	bool firstRow = center < resolution;
	bool lastRow = center > resolution * resolution - resolution;
	bool firstColumn = center % resolution == 0;
	bool lastColumn = center % resolution == resolution - 1;
	return closeIndexes[getIndex(center)];
}

const std::vector<char>& CloseIndexProvider::getTabIndexes(int center) const {
	return tabIndexes[getIndex(center)];
}

short CloseIndexProvider::getIndexAt(char index) const {
	return templateVec[index];
}
