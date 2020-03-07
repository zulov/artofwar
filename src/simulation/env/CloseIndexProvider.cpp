#include "CloseIndexProvider.h"

CloseIndexProvider::CloseIndexProvider(short res)
	: resolution(res),
	  templateVec{-res - 1, -res, -res + 1, -1, 1, res - 1, res, res + 1},
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
		  {}
	  } {

	for (int i = 0; i < 9; ++i) {
		for (auto j : tabIndexes[i]) {
			(closeIndexes[i]).emplace_back(templateVec[j]);
		}
	}
	int a =5;
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

const std::vector<short>& CloseIndexProvider::get(int center) const{
	return closeIndexes[getIndex(center)];
}

const std::vector<short>& CloseIndexProvider::getSecond(int center) const{
	return get(center);
}

const std::vector<char>& CloseIndexProvider::getTabIndexes(int center) const {
	return tabIndexes[getIndex(center)];
}

short CloseIndexProvider::getIndexAt(char index) const {
	return templateVec[index];
}
