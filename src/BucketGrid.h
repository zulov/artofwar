#pragma once
#include "Bucket.h"
class BucketGrid {
public:

	BucketGrid(double _resolution, double _size);
	Bucket* getBucketAt(int x, int y);
	void writeToGrid(std::vector<Entity *> crowd);
	void updateGrid(Entity * entity);
	std::vector <Entity *> getArrayNeight(Entity * entity);
	int getIndex(int _posX, int _posY);
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

