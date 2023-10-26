#pragma once
#include <span>
#include <vector>
#include "fiboheap.h"
#include "Urho3D/Math/Vector2.h"

namespace Urho3D {
	class Vector3;
	class Image;
}

struct GridCalculator;
struct CloseIndexes;
class ComplexBucketData;

//TODO perf zrobic takiego wstepnego findera o nizesz rozdzielczosci jezeli sa w jednym squere i tam nic niema to mozna isc prosto
class PathFinder {
public:
	PathFinder(short resolution, float size);

	~PathFinder();
	void setComplexBucketData(ComplexBucketData* complexData);
	const std::vector<int>* reconstructPath(int start, int goal, const int came_from[]) const;
	const std::vector<int>* reconstructSimplifyPath(int start, int goal, const int came_from[]) const;

	const std::vector<int>* findPath(int startIdx, int endIdx, int limit);
	const std::vector<int>* findPath(int startIdx, const std::vector<int>& endIdxs, int limit, bool closeEnough);

	void invalidateCache();
	void debug(int start, int end);
	void drawMap(Urho3D::Image* image) const;

private:
	float getDistCost(unsigned char neightIdx) const;

	const std::vector<int>* realFindPath(int startIdx, const std::vector<int>& endIdxs, int limit);
	const std::vector<int>* getClosePath2(int startIdx, int endIdx, const std::vector<short>& closePass) const;

	void prepareToStart(int startIdx);
	bool validateIndex(int current, int next) const;

	int getPassableEnd(int endIdx) const;
	std::vector<int> getPassableIndexes(const std::vector<int>& endIdxs, bool closeEnough) const;

	int heuristic(const Urho3D::IntVector2& from, const Urho3D::IntVector2& to) const;
	float heuristic(int from, std::vector<Urho3D::IntVector2>& endIdxs) const;

	bool ifInCache(int startIdx, int end) const { return lastStartIdx == startIdx && lastEndIdx == end; }
	bool ifInCache(int startIdx, const std::vector<int>& endIdxs) const;
	Urho3D::IntVector2 getCords(int index) const { return Urho3D::IntVector2(index / resolution, index % resolution); }
	std::vector<Urho3D::IntVector2> getCords(const std::vector<int>& endIdxs) const;
	void resetPathArrays();
	bool isInLocalArea(int center, int indexOfAim) const;
	bool isInLocal2Area(int center, int indexOfAim) const;
	int isInLocalArea(int center, std::vector<int>& endIdxs) const;
	void updateCost(int idx, float x);

	CloseIndexes* closeIndexes;
	GridCalculator* calculator;

	PriorityHeap frontier;

	std::vector<int>* tempPath = new std::vector<int>();
	std::vector<int>* closePath = new std::vector<int>();

	int lastStartIdx = -1;
	int lastEndIdx = -1;

	const float fieldSize;
	const float diagonalFieldSize;
	const short resolution;

	unsigned short staticCounter = 0;
	int* came_from;
	float* cost_so_far;
	ComplexBucketData* complexData;

	int min_cost_to_ref = 0;
	int max_cost_to_ref;
};
