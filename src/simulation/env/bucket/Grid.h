#pragma once
#include "BucketIterator.h"
#include "defines.h"
#include <Urho3D/Math/Vector3.h>

#define MAX_SEP_DIST 24
#define RES_SEP_DIST 120

class Unit;
class Bucket;
class Physical;

class Grid {
public:
	Grid(short _resolution, float _size);
	~Grid();

	void update(Unit* unit, char team) const;
	void update(Physical* entity) const;
	std::vector<Physical*>& getContentAt(int index);

	std::vector<short>* getEnvIndexsFromCache(float dist);
	std::vector<Physical *>* getArrayNeight(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair);
	std::vector<Physical *>* getArrayNeightSimilarAs(Physical* clicked, double radius);
	BucketIterator& getArrayNeight(Urho3D::Vector3& position, float radius, short thread);

	int indexFromPosition(Urho3D::Vector3& pos) const;
	int indexFromPosition(Urho3D::Vector2& pos) const;

	int getIndex(short posX, short posZ) const { return posX * resolution + posZ; }
protected:
	short diff(short a, short b);
	short getIndex(float value) const;
	bool inRange(int index) const { return index >= 0 && index < sqResolution; }
	
	Bucket* buckets;
	short resolution, halfResolution;
	int sqResolution;
	float size, fieldSize, invFieldSize;

private:
	bool fieldInCircle(short i, short j, float radius) const;
	std::vector<short>* getEnvIndexs(float radius) const;
	void addAt(int index, Physical* entity) const;
	void removeAt(int index, Physical* entity) const;

	float invDiff = RES_SEP_DIST / (float)MAX_SEP_DIST;

	BucketIterator iterators[MAX_THREADS];
	std::vector<short>* levelsCache[RES_SEP_DIST];
	std::vector<Physical*>* tempSelected;
	std::vector<Physical*> empty;
};
