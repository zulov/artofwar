#include "CloseIndexProvider.h"

CloseIndexProvider::CloseIndexProvider(short res)
	: resolution(res), closeindexes{
		  {1, res, res + 1},
		  {-1, 1, res - 1, res, res + 1},
		  {-1, res - 1, res},
		  {-res, -res + 1, 1, res, res + 1},
		  {-res - 1, -res, -res + 1, -1, 1, res - 1, res, res + 1},
		  {-res - 1, -res, -1, res - 1, res},
		  {-res, -res + 1, 1},
		  {-res - 1, -res, -res + 1, -1, 1},
		  {-res - 1, -res, -1}
	  } {
}

std::vector<short>& CloseIndexProvider::get(int current) {
	bool firstRow = current < resolution;
	bool lastRow = current > resolution * resolution - resolution;
	bool firstColumn = current % resolution == 0;
	bool lastColumn = current % resolution == resolution - 1;

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
	return closeindexes[index];
}
