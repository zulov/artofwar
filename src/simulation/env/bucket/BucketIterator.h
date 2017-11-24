#pragma once
#include <vector>
#include "objects/Physical.h"


class Grid;

class BucketIterator
{
public:
	BucketIterator();
	~BucketIterator();
	Unit* next();
	void init(std::vector<int>* _levels, int _center, Grid * _bucketGrid);
private:
	short index = 0;
	short secondIndex = 0;
	short sizeContent;
	short levelSize;
	std::vector<int>* levels;
	int center;

	std::vector<Unit *>* currentContent;
	Grid * bucketGrid;

};
