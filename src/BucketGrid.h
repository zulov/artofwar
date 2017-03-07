#pragma once
#include "Bucket.h"
#include <unordered_map>

class BucketGrid
{
public:

	BucketGrid(double _resolution, double _size);
	Bucket* getBucketAt(int x, int z);
	void writeToGrid(std::vector<Unit *>* crowd);
	void updateGrid(Unit* entity);
	std::vector<Unit *>* getArrayNeight(Unit* entity);
	int getIndex(int _posX, int _posZ);
	int calcXCordFromEdge(int index);
	int calcYCordFromEdge(int index);
	void updateSums();

	std::vector<std::vector<Bucket*>> getBucketList();
	void clean();
	void clearAfterStep();
private:

	int getIntegerPos(double value);
	int cacheKey(int dX, int dZ);
	std::vector<Bucket*> edgeBuckets;
	std::vector<std::vector<Bucket*>> bucketList;
	double resolution;
	double size;

	double maxContentCount;
	double maxSum;

	std::vector<Unit*>** cache;
	int lastSize = 10;
	int maxSize = 20;
};
