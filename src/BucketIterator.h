#pragma once
#include <utility>
#include <vector>
#include "Physical.h"


class BucketGrid;

class BucketIterator
{
public:
	BucketIterator(std::vector<std::pair<short, short>*>* _levels, short _dX, short _dZ, BucketGrid * _bucketGrid);
	~BucketIterator();
	Physical* next();
private:
	short index = 0;
	short secondIndex = 0;
	short sizeContent;
	short levelSize;
	std::vector<std::pair<short, short>*>* levels;
	short dX;
	short dZ;
	std::vector<Physical *>* currentContent;
	BucketGrid * bucketGrid;

};
