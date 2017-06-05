#pragma once
#include <utility>
#include <vector>
#include "Entity.h"
#include <list>


class BucketGrid;

class BucketIterator
{
public:
	BucketIterator(std::vector<std::pair<int, int>*>* _levels, int _dX, int _dZ, BucketGrid * _bucketGrid);
	~BucketIterator();
	Entity* next();
private:
	int index = 0;
	int secondIndex = 0;
	int sizeContent;
	int levelSize;
	std::vector<std::pair<int, int>*>* levels;
	int dX;
	int dZ;
	std::vector<Entity *>* currentContent;
	BucketGrid * bucketGrid;

};
