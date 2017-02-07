#pragma once
#include "Bucket.h"
class BucketGrid {
public:

	BucketGrid(double _resolution, double _size);
	Bucket* getBucketAt(int x, int z);
	void writeToGrid(std::vector<Unit *> crowd);
	void updateGrid(Unit * entity);
	std::vector <Unit *> getArrayNeight(Unit * entity);
	int getIndex(int _posX, int _posZ);
	int calcXCordFromEdge(int index);
	int calcYCordFromEdge(int index);
	void updateSums();

	std::vector<std::vector<Bucket*>> getBucketList();
	void clean();
private:
	

	int getIntegerPos(double value);
	std::vector<Bucket*> edgeBuckets;
	std::vector<std::vector<Bucket*>> bucketList;
	double resolution;
	double size;

	double maxContentCount;
	double maxSum;
};

