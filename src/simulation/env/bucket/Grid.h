#pragma once
#include <Urho3D/Math/Vector3.h>
#include "BucketIterator.h"
#include "vector"
#include "simulation/env/CloseIndexProvider.h"
#include "utils/defines.h"

namespace Urho3D { class Vector2; }
struct GridCalculator;

constexpr char MAX_SEP_DIST = 24;
constexpr char RES_SEP_DIST = 120;

class Unit;
class Bucket;
class Physical;

class Grid {
public:
	Grid(short resolution, float size);
	Grid(const Grid& rhs) = delete;
	~Grid();

	void update(Unit* unit, char team) const;
	void update(Physical* entity) const;
	const std::vector<Physical*>& getContentAt(int index) const;

	std::vector<short>* getEnvIndexesFromCache(float dist);
	std::vector<Physical*>* getArrayNeight(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair, char player) const;
	std::vector<Physical*>* getArrayNeightSimilarAs(Physical* clicked, float radius);
	std::vector<int> getArrayNeight(const Urho3D::Vector2& center, float radius) const;
	BucketIterator& getArrayNeight(Urho3D::Vector3& position, float radius, short thread);
	const std::vector<short>& getCloseIndexes(int center) const;
	const std::vector<char>& getCloseTabIndexes(short center) const;
protected:
	GridCalculator* calculator;
	CloseIndexProvider closeIndexProvider;

	Bucket* buckets;
	short resolution;
	int sqResolution;
	float size, fieldSize, invFieldSize;

private:
	const std::vector<Physical*>& getContentAt(short x, short z) const;
	const std::vector<Physical*>& getNotSafeContentAt(short x, short z) const;
	bool fieldInCircle(short i, short j, float radius) const;
	std::vector<short>* getEnvIndexs(float radius, std::vector<short>* prev) const;
	void addAt(int index, Physical* entity) const;
	void removeAt(int index, Physical* entity) const;

	float invDiff = RES_SEP_DIST / (float)MAX_SEP_DIST;

	BucketIterator iterators[MAX_THREADS];
	std::vector<short>* levelsCache[RES_SEP_DIST];
	std::vector<Physical*>* tempSelected;
};
