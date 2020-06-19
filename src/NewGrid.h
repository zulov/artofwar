#pragma once
#include "AbstractGrid.h"
#include "NewBucket.h"
#include "NewBucketIterator.h"
#include "objects/Physical.h"
#include "simulation/env/CloseIndexProvider.h"
#include "simulation/env/GridCalculator.h"
#include "utils/defines.h"

#define MAX_SEP_DIST 24
#define RES_SEP_DIST 120

template <class T>
class NewGrid :public AbstractGrid<T>{
public:
	NewGrid(short resolution, float size);
	~NewGrid();

//	void update(T* unit, char team) const;
	void update(T* entity) const;
	std::vector<T*>& getContentAt(int index);

	std::vector<short>* getEnvIndexesFromCache(float dist);
	std::vector<T*>* getArrayNeight(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair, char player);
	std::vector<int> getArrayNeight(const Urho3D::Vector2& center, float radius) const;
	std::vector<T*>* getArrayNeightSimilarAs(Physical* clicked, double radius);
	NewBucketIterator<T>& getArrayNeight(Urho3D::Vector3& position, float radius, short thread);
	const std::vector<short>& getCloseIndexes(int center) const;
	const std::vector<char>& getCloseTabIndexes(short center) const;
protected:
	bool inRange(int index) const { return index >= 0 && index < sqResolution; }
	GridCalculator calculator;
	CloseIndexProvider closeIndexProvider;

	NewBucket<T>* buckets;
	int sqResolution;
	float size, fieldSize, invFieldSize;
	short resolution;

private:
	std::vector<T*>& getContentAt(short x, short z);
	bool fieldInCircle(short i, short j, float radius) const;
	std::vector<short>* getEnvIndexs(float radius) const;
	void addAt(int index, T* entity) const;
	void removeAt(int index, T* entity) const;

	float invDiff = RES_SEP_DIST / (float)MAX_SEP_DIST;

	NewBucketIterator<T> iterators[MAX_THREADS];
	std::vector<short>* levelsCache[RES_SEP_DIST];
	std::vector<T*>* tempSelected;
	std::vector<T*> empty;
};
