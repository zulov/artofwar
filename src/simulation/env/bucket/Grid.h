#pragma once
#include "Bucket.h"
#include "BucketIterator.h"

#define MAX_SEP_DIST 16
#define RES_SEP_DIST 80


class Grid
{
public:
	Grid(short _resolution, float _size, bool _debugEnabled = false);
	~Grid();

	void updateGrid(Unit* entity, char team);
	std::vector<Unit*>& getContentAt(int index);
	
	std::vector<short>* getEnvIndexsFromCache(float dist);
	std::vector<Physical *>* getArrayNeight(std::pair<Vector3*, Vector3*>& pair);
	BucketIterator& getArrayNeight(Unit* entity, float radius, short thread);
	int getIndex(short posX, short posZ);
protected:
	int indexFromPosition(Vector3* position);
	short getIndex(float value) const;
	bool inRange(int index);
	short resolution;
	int sqResolution;
	float size;
	float fieldSize;
	bool debugEnabled;
	short halfResolution;
	float invFieldSize;

	Bucket* buckets;

private:
	bool fieldInCircle(short i, short j, float radius);
	std::vector<short>* getEnvIndexs(float radius);
	void addAt(int index, Unit* entity);
	void removeAt(int index, Unit* entity);

	float invDiff = RES_SEP_DIST / (float)MAX_SEP_DIST;

	BucketIterator iterators[MAX_THREADS];
	std::vector<short>* levelsCache[RES_SEP_DIST];
	std::vector<Unit*> empty;

	std::vector<Physical*>* tempSelected;
};
