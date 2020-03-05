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
	  } {}

std::vector<short>& CloseIndexProvider::get(int current) {}
