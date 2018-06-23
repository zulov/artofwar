#pragma once
#include "BucketIterator.h"
#include "defines.h"
#include <Urho3D/Math/Vector3.h>

#define MAX_SEP_DIST 16
#define RES_SEP_DIST 80

class Bucket;
class Physical;

class Grid
{
public:
	Grid(short _resolution, float _size);
	~Grid();

	void update(Physical* entity, char team) const;
	std::vector<Physical*>& getContentAt(int index);

	std::vector<short>* getEnvIndexsFromCache(float dist);
	std::vector<Physical *>* getArrayNeight(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair);
	BucketIterator& getArrayNeight(Physical* entity, float radius, short thread);
	
	int indexFromPosition(Urho3D::Vector3* position) const;
	int indexFromPosition(Urho3D::Vector2& position) const;

	int getIndex(short posX, short posZ) const { return posX * resolution + posZ; }
protected:
	short getIndex(float value) const;
	bool inRange(int index) const {return index >= 0 && index < sqResolution;}
	short resolution;
	int sqResolution;
	float size;
	float fieldSize;
	short halfResolution;
	float invFieldSize;

	Bucket* buckets;

private:
	bool fieldInCircle(short i, short j, float radius) const;
	std::vector<short>* getEnvIndexs(float radius) const;
	void addAt(int index, Physical* entity) const;
	void removeAt(int index, Physical* entity) const;

	float invDiff = RES_SEP_DIST / (float)MAX_SEP_DIST;

	BucketIterator iterators[MAX_THREADS];
	std::vector<short>* levelsCache[RES_SEP_DIST];
	std::vector<Physical*> empty;

	std::vector<Physical*>* tempSelected;
};
