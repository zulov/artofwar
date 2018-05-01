#pragma once
#include "objects/Physical.h"
#include <vector>


class Grid;

class BucketIterator
{
public:
	BucketIterator();
	~BucketIterator();
	Unit* next();
	void init(std::vector<short>* _levels, int _center, Grid* _bucketGrid);
private:
	short index = 0;

	short levelSize;
	std::vector<short>* levels;
	int center;
	std::vector<Unit *>::iterator currentIterator;
	std::vector<Unit *>::iterator currentEnd;
	Grid* bucketGrid;
};
