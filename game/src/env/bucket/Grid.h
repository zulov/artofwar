#pragma once
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
	Grid(short resolution, float size, float maxQueryRadius);
	Grid(const Grid& rhs) = delete;
	virtual ~Grid();

	virtual int update(Unit* unit, int currentIndex, bool shouldChangeFlag) const;

	virtual void remove(Physical* physical) const;

	void removeAt(int index, Physical* entity) const;

	virtual int updateNew(Physical* physical) const;

	const std::vector<Physical*>& getContentAt(int index) const;

	std::vector<Physical*>* getArrayNeight(MouseHeld& held, char player);
	std::vector<Physical*>* getArrayNeightSimilarAs(Physical* clicked, float radius);
	std::vector<int> getCloseCenters(Urho3D::Vector2& center, float radius) const;
	void invalidateCache();
	void invalidateCache(int currentIdx, float radius);
	BucketIterator& getArrayNeight(const Urho3D::Vector3& position, float radius);
	BucketIterator& getArrayNeight(int center, float radius);

	bool onlyOneInside(int index) const;
	std::vector<Physical*>* getAllFromCache(int currentIdx, float radius);
	void addFromCell(short shiftIdx, int currentIdx) const;
	std::vector<Physical*>* getAll(int currentIdx, float radius);

protected:
	GridCalculator* calculator;
	CloseIndexes* closeIndexes;

	Bucket* buckets;
	LevelCache* levelCache;
	int resolution;
	int sqResolution;

private:
	const std::vector<Physical*>& getNotSafeContentAt(short x, short z) const;

	void addAt(int index, Physical* entity) const;

	BucketIterator iterator;

	std::vector<Physical*>* tempSelected;
	std::vector<Physical*>* cache;
	int prevIndex = -1;
	float prevRadius = -1.f;
};
