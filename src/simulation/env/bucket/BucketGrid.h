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

	std::vector<Unit *>* getContentAt(short posX, short posZ);
	void updateGrid(Unit* entity, short team);
	bool validateAdd(Static* object);
	void addStatic(Static* object);
	void removeStatic(Static* object);

	std::vector<std::pair<char, char>>* getEnvIndexsFromCache(double getMaxSeparationDistance);
	BucketIterator* getArrayNeight(Unit* entity, double radius, short thread);
	std::vector<Physical *>* getArrayNeight(std::pair<Vector3*, Vector3*>* pair);
	Vector3* validatePosition(Vector3* position);
private:
	BucketIterator** iterators;
	bool fieldInCircle(int i, int j, double radius);
	std::vector<std::pair<char, char>>* getEnvIndexs(double radius);
	short getIndex(double value);
	bool isInSide(short _posX, short _posZ) const;
	void addAt(short posX, short posZ, Unit* entity);
	void removeAt(short posX, short posZ, Unit* entity);

	Bucket** buckets;
	short resolution;
	short halfResolution;
	double size;
	double fieldSize;

	double diff = ((double)MAX_SEP_DIST) / RES_SEP_DIST;

	std::vector<std::pair<char, char>>** levelsCache;
	std::vector<Unit*>* empty;
};
