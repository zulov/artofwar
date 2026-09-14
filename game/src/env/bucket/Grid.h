#pragma once
#include <span>

#include "BucketIterator.h"
#include "objects/building/Building.h"

struct MouseHeld;

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
	Grid(unsigned short resolution, float size, float maxQueryRadius);
	Grid(const Grid& rhs) = delete;
	virtual ~Grid();

	virtual int update(Unit* unit, int currentIndex, bool shouldChangeFlag) const;

	virtual void remove(Physical* physical) const;

	void removeAt(int index, Physical* entity) const;

	virtual int updateNew(Physical* physical) const;

	std::span<Physical* const> getContentAt(int index) const;
	void appendIndexesInRange(const Urho3D::Vector2& center, float radius, std::vector<int>& indexes) const;

	std::vector<Physical*>* getArrayNeight(MouseHeld& held, char player);
	std::vector<Physical*>* getArrayNeightSimilarAs(const Urho3D::Vector2& center, unsigned short databaseId,
	                                               char playerId, float radius);
	std::vector<int> getCloseCenters(Urho3D::Vector2& center, float radius) const;
	BucketIterator& getArrayNeight(const Urho3D::Vector2& position, float radius);
	BucketIterator& getArrayNeight(int center, float radius);

protected:
	GridCalculator* calculator;
	CloseIndexes* closeIndexes;

	Bucket* buckets;
	LevelCache* levelCache;
	int resolution;
	int sqResolution;

private:
	std::span<Physical* const> getNotSafeContentAt(short x, short z) const;

	void addAt(int index, Physical* entity) const;

	BucketIterator iterator;

	std::vector<Physical*>* tempSelected;
};
