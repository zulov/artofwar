#pragma once

#include <vector>

#include "PriorityHeap.h"
#include "env/GridCalculator.h"
#include "Urho3D/Math/Vector2.h"

struct PathCache;

namespace Urho3D {
	class Vector3;
	class Image;
}

struct GridCalculator;
struct CloseIndexes;
class ComplexBucketData;

constexpr int PATH_PRECISION = 20;

class PathFinder {
public:
	PathFinder(short resolution, float size);

	~PathFinder();
	void setComplexBucketData(ComplexBucketData* complexData);
	const std::vector<int>* reconstructPath(int start, int goal) const;
	const std::vector<int>* reconstructSimplifyPath(int start, int goal) const;

	const std::vector<int>* findPath(int startIdx, const std::vector<int>& endIdxs);

	void invalidateCache();
	void drawMap(Urho3D::Image* image) const;

private:

	const std::vector<int>* realFindPath(int startIdx, const std::vector<int>& endIdxs);
	const std::vector<int>* getClosePath2(int startIdx, int endIdx, const std::vector<short>& closePass) const;

	void prepareToStart(int startIdx);
	int heuristic(int from, std::vector<Urho3D::IntVector2>& endIdxs) const;

	int findInCache(int start, int end) const;
	void addToCache(int startIdx, int endIdx, const std::vector<int>* vector);

	Urho3D::IntVector2 getCords(int index) const { return calculator->getCords(index); }
	std::vector<Urho3D::IntVector2> getCords(const std::vector<int>& endIdxs) const;
	void resetPathArrays();
	bool isInLocalArea(int center, int indexOfAim) const;
	bool isInLocal2Area(int center, int indexOfAim) const;

	void update(int idx, int cost, int cameForm, int heuristicCost);

	void debug(int start, int end, bool pathFound);
	bool validateIndex(int current, int next) const;

	CloseIndexes* closeIndexes;
	GridCalculator* calculator;

	PriorityHeap frontier;

	std::vector<int>* tempPath = new std::vector<int>();
	std::vector<int>* closePath = new std::vector<int>();

	PathCache* cache;
	char cacheIdx = 0;
	char cacheSize = 0;

	const short resolution;
	const int sqResolution;

	unsigned short staticCounter = 0;
	int* came_from;
	int* cost_so_far;
	ComplexBucketData* complexData;

	int min_cost_to_ref = 0;
	int max_cost_to_ref;
	int distances[8] = {
		int(sqrtf(2) * PATH_PRECISION), PATH_PRECISION, int(sqrtf(2) * PATH_PRECISION),
		PATH_PRECISION, PATH_PRECISION,
		int(sqrtf(2) * PATH_PRECISION), PATH_PRECISION, int(sqrtf(2) * PATH_PRECISION)
	};
};
