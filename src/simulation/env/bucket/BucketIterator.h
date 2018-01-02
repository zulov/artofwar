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
	void init(std::vector<short>* _levels, int _center, Grid * _bucketGrid);
private:
	short index = 0;
	short secondIndex = 0;
	short sizeContent;
	short levelSize;
	std::vector<short>* levels;
	int center;

	std::vector<Unit *>* currentContent{};
	Grid * bucketGrid;

};
