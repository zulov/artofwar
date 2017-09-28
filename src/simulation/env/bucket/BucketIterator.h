#pragma once
#include <utility>
#include <vector>
#include "objects/Physical.h"


class BucketGrid;

class BucketIterator
{
public:
	BucketIterator();
	~BucketIterator();
	Unit* next();
	void init(std::vector<int>* _levels, int _center, BucketGrid * _bucketGrid);
private:
	short index = 0;
	short secondIndex = 0;
	short sizeContent;
	short levelSize;
	std::vector<int>* levels;
	int center;

	std::vector<Unit *>* currentContent;
	BucketGrid * bucketGrid;

};
