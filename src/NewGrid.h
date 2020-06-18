#pragma once
#include "NewBucket.h"
#include "NewBucketIterator.h"

template <class T>
class NewGrid {
public:
	NewGrid(short resolution, float size);
	~NewGrid();

	void update(T* unit, char team) const;
	void update(T* entity) const;
	std::vector<T*>& getContentAt(int index);

	std::vector<short>* getEnvIndexesFromCache(float dist);
	std::vector<T*>* getArrayNeight(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair, char player);
	std::vector<T*>* getArrayNeightSimilarAs(Physical* clicked, double radius);
	std::vector<int> getArrayNeight(const Urho3D::Vector2& center, float radius) const;
	NewBucketIterator<T>& getArrayNeight(Urho3D::Vector3& position, float radius, short thread);
	const std::vector<short>& getCloseIndexes(int center) const;
	const std::vector<char>& getCloseTabIndexes(short center) const;
protected:
	GridCalculator calculator;
	CloseIndexProvider closeIndexProvider;
	bool inRange(int index) const { return index >= 0 && index < sqResolution; }

	NewBucket<T>* buckets;
	short resolution;
	int sqResolution;
	float size, fieldSize, invFieldSize;

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
