#pragma once
#include "Bucket.h"

#define MAX_SEP_DIST 20
#define RES_SEP_DIST 200
class BucketIterator;

class BucketGrid
{
public:
	BucketGrid(short _resolution, double _size, bool _debugEnabled = false);
	~BucketGrid();

	void updateGrid(Unit* entity, short team);
	bool validateAdd(Static* object);
	void addStatic(Static* object);
	void removeStatic(Static* object);
	std::vector<Unit*>* getContentAt(int index);

	std::vector<int>* getEnvIndexsFromCache(double getMaxSeparationDistance);
	BucketIterator* getArrayNeight(Unit* entity, double radius, short thread);

	std::vector<Physical *>* getArrayNeight(std::pair<Vector3*, Vector3*>* pair);
	int getIndex(short posX, short posZ);
	Vector3* validatePosition(Vector3* position);

private:
	BucketIterator** iterators;
	bool fieldInCircle(short i, short j, double radius);
	std::vector<int>* getEnvIndexs(double radius);
	short getIndex(double value);
	void addAt(int index, Unit* entity);
	bool inRange(int index);
	void removeAt(int index, Unit* entity);

	Bucket* buckets;
	Node** boxes;
	short resolution;
	short halfResolution;
	double size;
	double fieldSize;
	bool debugEnabled;

	double diff = ((double)MAX_SEP_DIST) / RES_SEP_DIST;

	std::vector<int>** levelsCache;
	std::vector<Unit*>* empty;
};
