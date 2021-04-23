#pragma once
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
	PathFinder(short resolution, float size, ComplexBucketData* complexData);

	~PathFinder();
	std::vector<int>* reconstruct_path(int start, int goal, const int came_from[]) const;
	std::vector<int>* reconstruct_simplify_path(int start, int goal, const int came_from[]) const;
	
	std::vector<int>* findPath(int startIdx, int endIdx, int limit);
	std::vector<int>* findPath(int startIdx, const std::vector<int>& endIdxs, int limit);

	std::vector<int>* findPath(int startIdx, const Urho3D::Vector2& aim, int limit);

	void refreshWayOut(std::vector<int>& toRefresh);

	void invalidateCache();
	void debug(int start, int end);
	void drawMap(Urho3D::Image* image) const;
	void prepareGridToFind();
private:
	std::vector<int>* realFindPath(int startIdx, int endIdx, int limit);
	std::vector<int>* realFindPath(int startIdx, const std::vector<int>& endIdxs, int limit);

	void prepareToStart(int startIdx);
	void validateIndex(int current, int next) const;
	
	int getPassableEnd(int endIdx) const;
	std::vector<int> getPassableIndexes(const std::vector<int>& endIdxs) const;

	float heuristic(int from, const Urho3D::IntVector2& to) const;
	float heuristic(int from, const std::vector<int>& endIdxs) const;

	bool ifInCache(int startIdx, int end) const { return lastStartIdx == startIdx && lastEndIdx == end; }
	bool ifInCache(int startIdx, const std::vector<int>& endIdxs) const;
	Urho3D::IntVector2 getCords(int index) const { return Urho3D::IntVector2(index / resolution, index % resolution); }
	void resetPathArrays();
	bool isInLocalArea(int center, int indexOfAim) const;
	bool isInLocal2Area(int center, int indexOfAim) const;
	int isInLocalArea(int center, std::vector<int>& endIdxs) const;
	void updateCost(int startIdx, float x);

	CloseIndexes* closeIndexes;
	GridCalculator* calculator;

	std::vector<int>* tempPath;
	std::vector<int>* closePath;

	int lastStartIdx = -1;
	int lastEndIdx = -1;

	const short resolution;
	bool pathInited = false;
	const float fieldSize;

	int* came_from;
	float* cost_so_far;
	ComplexBucketData* complexData;

	int min_cost_to_ref = 0;
	int max_cost_to_ref;
	unsigned short staticCounter = 0;

	FibHeap frontier;
};
