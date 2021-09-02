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
	Grid(short resolution, float size, bool initCords, float maxQueryRadius);
	Grid(const Grid& rhs) = delete;
	virtual ~Grid();

	//virtual void updateSparse(Unit* unit, char team) const;
	virtual int update(Physical* physical, int currentIndex) const;

	//virtual void remove(Unit* unit, char team) const;
	//virtual void remove(Physical* physical) const;

	void removeAt(int index, Physical* entity) const;

	//virtual int updateNew(Unit* unit, char team) const;
	virtual int updateNew(Physical* physical) const;

	const std::vector<Physical*>& getContentAt(int index) const;

	std::vector<Physical*>* getArrayNeight(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair, char player);
	std::vector<Physical*>* getArrayNeightSimilarAs(Physical* clicked, float radius);
	std::vector<int> getCloseCenters(Urho3D::Vector2& center, float radius) const;
	void invalidateCache();
	void invalidateCache(int currentIdx, float radius);
	BucketIterator& getArrayNeight(const Urho3D::Vector3& position, float radius);
	BucketIterator& getArrayNeight(int center, float radius);
	const std::vector<short>& getCloseIndexes(int center) const;
	const std::vector<unsigned char>& getCloseTabIndexes(int center) const;

	bool onlyOneInside(int index) const;
	std::vector<Physical*>* getAllFromCache(int currentIdx, float radius);
	std::vector<Physical*>* getAll(int currentIdx, float radius);

protected:
	GridCalculator* calculator;
	CloseIndexes* closeIndexes;

	Bucket* buckets;
	int sqResolution;
	LevelCache* levelCache;

private:
	const std::vector<Physical*>& getNotSafeContentAt(short x, short z) const;

	void addAt(int index, Physical* entity) const;

	BucketIterator iterator;

	std::vector<Physical*>* tempSelected;
	int prevIndex = -1;
	float prevRadius = -1.f;
};
