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
	const std::vector<Physical*>& getContentAt(int index) const;

	std::vector<Physical*>* getArrayNeight(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair, char player) const;
	std::vector<Physical*>* getArrayNeightSimilarAs(Physical* clicked, float radius);
	std::vector<int> getArrayNeight(const Urho3D::Vector2& center, float radius) const;
	BucketIterator& getArrayNeight(Urho3D::Vector3& position, float radius);
	const std::vector<short>& getCloseIndexes(int center) const;
	const std::vector<char>& getCloseTabIndexes(short center) const;
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
};
