#pragma once
#include "Bucket.h"
#include <unordered_map>
#include <iostream>

#define MAX_SEP_DIST 12
#define RES_SEP_DIST 100
class BucketIterator;

class BucketGrid
{
public:

	BucketGrid(int _resolution, double _size);
	~BucketGrid();
	Bucket* getBucketAt(int x, int z);
	void updateGrid(Entity* entity, int team);
	std::vector<std::pair<int, int>*>* getEnvIndexsFromCache(double getMaxSeparationDistance);
	BucketIterator* getArrayNeight(Unit* entity, double radius);

	void clean();
	std::vector<Entity *>* getArrayNeight(std::pair<Entity*, Entity*>* pair);
private:
	bool fieldInCircle(int i, int j, double radius);
	std::vector<std::pair<int, int>*>* getEnvIndexs(double radius);
	int getIntegerPos(double value);
	void updateSizes(int size);
	bool isInSide(int _posX, int _posZ) const;
	Bucket*** bucketList;
	int resolution;
	int halfResolution;
	double size;
	double fieldSize;

	double diff = ((double)MAX_SEP_DIST) / RES_SEP_DIST;

	std::vector<std::pair<int, int>*>** levelsCache;
	int lastSize = 10;
	int maxSize = 20;
	Bucket *empty;
};
