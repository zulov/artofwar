#pragma once
#include "Bucket.h"
#include <unordered_map>

class BucketGrid
{
public:

	BucketGrid(int _resolution, double _size);
	Bucket* getBucketAt(int x, int z);
	void writeToGrid(std::vector<Unit *>* crowd);
	void updateGrid(Unit* entity);
	std::vector<Unit *>* getArrayNeight(Unit* entity);

	void clean();
	void clearAfterStep();
private:
	int getIntegerPos(double value);
	int cacheHash(int dX, int dZ);
	void updateSizes(int size);
	bool isInSide(int _posX, int _posZ) const;
	std::vector<std::vector<Bucket*>> bucketList;
	int resolution;
	double size;

	std::vector<Unit*>** cache;
	int lastSize = 10;
	int maxSize = 20;
};
