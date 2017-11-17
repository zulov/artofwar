#pragma once
#include "Bucket.h"

#define MAX_SEP_DIST 20
#define RES_SEP_DIST 200
class BucketIterator;

class Grid
{
public:
	Grid(short _resolution, double _size, bool _debugEnabled = false);
	~Grid();


	void updateGrid(Unit* entity, short team);
	std::vector<Unit*>* getContentAt(int index);
	int getSizeAt(int index);
	std::vector<int>* getEnvIndexsFromCache(double getMaxSeparationDistance);
	std::vector<Physical *>* getArrayNeight(std::pair<Vector3*, Vector3*>* pair);
	BucketIterator* getArrayNeight(Unit* entity, double radius, short thread);
	int getIndex(short posX, short posZ);
protected:
	short getIndex(double value);
	bool inRange(int index);
	short resolution;
	double fieldSize;
	bool debugEnabled;
	short halfResolution;
	double size;

	Bucket* buckets;

private:
	BucketIterator** iterators;
	bool fieldInCircle(short i, short j, double radius);
	std::vector<int>* getEnvIndexs(double radius);
	void addAt(int index, Unit* entity);
	void removeAt(int index, Unit* entity);

	Node** boxes;
	double diff = ((double)MAX_SEP_DIST) / RES_SEP_DIST;

	std::vector<int>** levelsCache;
	std::vector<Unit*>* empty;
};
