#pragma once
#include "BucketIterator.h"


namespace Urho3D {
	class Vector2;
	class Vector3;
}

class CloseIndexes;
class LevelCache;
class Unit;
class Bucket;
class Physical;
struct GridCalculator;

class Grid {
public:
	Grid(short resolution, float size, float maxQueryRadius);
	Grid(const Grid& rhs) = delete;
	~Grid();

	void update(Unit* unit, char team) const;
	void update(Physical* entity) const;

	void remove(Unit* unit, char team) const;
	void remove(Physical* entity) const;

	void updateNew(Physical* physical) const;
	void updateNew(Unit* unit, char team) const;

	const std::vector<Physical*>& getContentAt(int index) const;

	std::vector<Physical*>* getArrayNeight(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair, char player);
	std::vector<Physical*>* getArrayNeightSimilarAs(Physical* clicked, float radius);
	std::vector<int> getArrayNeight(const Urho3D::Vector2& center, float radius) const;
	void invalidateCache();
	void invalidateCache(const int currentIdx, float radius);
	BucketIterator& getArrayNeight(Urho3D::Vector3& position, float radius);
	BucketIterator& getArrayNeight(int center, float radius);
	const std::vector<short>& getCloseIndexes(int center) const;
	const std::vector<unsigned char>& getCloseTabIndexes(short center) const;

	bool onlyOneInside(int index) const;
	std::vector<Physical*>* getAllFromCache(int currentIdx, float radius);
	std::vector<Physical*>* getAll(int currentIdx, float radius);

protected:
	GridCalculator* calculator;
	CloseIndexes* closeIndexes;

	Bucket* buckets;
	short resolution;
	int sqResolution;

private:
	const std::vector<Physical*>& getContentAt(short x, short z) const;
	const std::vector<Physical*>& getNotSafeContentAt(short x, short z) const;

	void addAt(int index, Physical* entity) const;
	void removeAt(int index, Physical* entity) const;

	BucketIterator iterator;
	LevelCache* levelCache;

	std::vector<Physical*>* tempSelected;
	int prevIndex = -1;
	float prevRadius = -1.f;
};
