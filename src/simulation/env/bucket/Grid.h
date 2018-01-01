#pragma once
#include "Bucket.h"

#define MAX_SEP_DIST 16
#define RES_SEP_DIST 80
class BucketIterator;

class Grid
{
public:
	Grid(short _resolution, double _size, bool _debugEnabled = false);
	~Grid();

	void updateGrid(Unit* entity, char team);
	std::vector<Unit*>& getContentAt(int index);
	int &getSizeAt(int index);
	std::vector<short>* getEnvIndexsFromCache(double dist);
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
	double invFieldSize;

	Bucket* buckets;

private:
	bool fieldInCircle(short i, short j, double radius);
	std::vector<short>* getEnvIndexs(double radius);
	void addAt(int index, Unit* entity);
	void removeAt(int index, Unit* entity);

	double diff = ((double)MAX_SEP_DIST) / RES_SEP_DIST;

	BucketIterator* iterators[MAX_THREADS];
	std::vector<short>* levelsCache[RES_SEP_DIST];
	std::vector<Unit*> empty;

	std::vector<Physical*>* tempSelected;
};
