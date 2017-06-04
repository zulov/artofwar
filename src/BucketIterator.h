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
	std::vector<std::pair<int, int>*>* levels;
	int dX;
	int dZ;
	std::list<Entity*>::const_iterator iterator;
	std::list<Entity *>* currentContent;
	BucketGrid * bucketGrid;

};
