#pragma once
#include "Bucket.h"
#include <unordered_map>

#define MAX_SEP_DIST 10
#define RES_SEP_DIST 100

class BucketGrid
{
public:

	BucketGrid(int _resolution, double _size);
	~BucketGrid();
	Bucket* getBucketAt(int x, int z);
	void writeToGrid(std::vector<Unit *>* crowd);
	void updateGrid(Entity* entity);
	std::vector<std::pair<int, int>*>* getEnvIndexsFromCache(double getMaxSeparationDistance);
	std::vector<Entity *>* getArrayNeight(Unit* entity);

	void clean();
	void clearAfterStep();
	std::vector<Entity *>* getArrayNeight(std::pair<Entity*, Entity*>* pair);
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

	std::vector<Entity*>** cache;
	std::vector<std::pair <int, int>*>** levelsCache;
	int lastSize = 10;
	int maxSize = 20;
};
