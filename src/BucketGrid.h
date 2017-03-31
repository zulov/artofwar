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
	bool fieldInCircle(int i, int j, double radius);
	std::vector<std::pair <int, int>*>* getEnvIndexs(double radius);
	int getIntegerPos(double value);
	int cacheHash(int dX, int dZ);
	void updateSizes(int size);
	bool isInSide(int _posX, int _posZ) const;
	std::vector<std::vector<Bucket*>> bucketList;
	int resolution;
	double size;
	double fieldSize;

	std::vector<Unit*>** cache;
	int lastSize = 10;
	int maxSize = 20;
};
