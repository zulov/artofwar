#pragma once
#include <Urho3D/Math/Vector3.h>


#include "BucketIterator.h"
#include "simulation/env/CloseIndexProvider.h"
#include "simulation/env/GridCalculator.h"
#include "utils/defines.h"

#define MAX_SEP_DIST 24
#define RES_SEP_DIST 120

class Unit;
class Bucket;
class Physical;

class Grid {
public:
	Grid(short resolution, float size);
	~Grid();

	void update(Unit* unit, char team) const;
	void update(Physical* entity) const;
	std::vector<Physical*>& getContentAt(int index);

	std::vector<short>* getEnvIndexesFromCache(float dist);
	std::vector<Physical*>* getArrayNeight(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair, char player);
	std::vector<Physical*>* getArrayNeightSimilarAs(Physical* clicked, double radius);
	std::vector<int> getArrayNeight(const Urho3D::Vector2& center, float radius) const;
	BucketIterator& getArrayNeight(Urho3D::Vector3& position, float radius, short thread);
	const std::vector<short>& getCloseIndexes(int center) const;
	const std::vector<char>& getCloseTabIndexes(short center) const;
protected:
	GridCalculator calculator;
	CloseIndexProvider closeIndexProvider;
	bool inRange(int index) const { return index >= 0 && index < sqResolution; }

	Bucket* buckets;
	short resolution;
	int sqResolution;
	float size, fieldSize, invFieldSize;

private:
	std::vector<Physical*>& getContentAt(short x, short z);
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
