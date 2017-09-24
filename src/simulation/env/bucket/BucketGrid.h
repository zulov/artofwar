#pragma once
#include "Bucket.h"
#include <unordered_map>

#define MAX_SEP_DIST 20
#define RES_SEP_DIST 200
class BucketIterator;

class BucketGrid
{
public:
	BucketGrid(short _resolution, double _size);
	~BucketGrid();

	std::vector<Unit *>* getContentAt(short x, short z);
	void updateGrid(Unit* entity, short team);
	bool validateAdd(Static* object);
	void addStatic(Static* object);
	void removeStatic(Static* object);

	std::vector<std::pair<short, short>*>* getEnvIndexsFromCache(double getMaxSeparationDistance);
	BucketIterator* getArrayNeight(Unit* entity, double radius, short thread);
	std::vector<Physical *>* getArrayNeight(std::pair<Vector3*, Vector3*>* pair);
	Vector3* validatePosition(Vector3* position);
private:
	BucketIterator** iterators;
	bool fieldInCircle(int i, int j, double radius);
	std::vector<std::pair<short, short>*>* getEnvIndexs(double radius);
	int getIntegerPos(double value);
	bool isInSide(int _posX, int _posZ) const;
	void addAt(short x, short z, Unit* entity);
	void removeAt(short x, short z, Unit* entity);

	Bucket** buckets;
	short resolution;
	short halfResolution;
	double size;
	double fieldSize;

	double diff = ((double)MAX_SEP_DIST) / RES_SEP_DIST;

	std::vector<std::pair<short, short>*>** levelsCache;
	std::vector<Unit*>* empty;
};
