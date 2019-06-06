#pragma once
#include <vector>
#include <Urho3D/Math/Vector2.h>
#include "BucketQueue.h"
#include <Urho3D/Resource/Image.h>

class ComplexBucketData;

class PathFinder {
public:
	PathFinder(short resolution, float size, ComplexBucketData* complexData);

	~PathFinder();
	std::vector<int>* reconstruct_path(int start, int goal, const int came_from[]) const;
	std::vector<int>* reconstruct_simplify_path(int start, int goal, const int came_from[]) const;
	std::vector<int>* findPath(int startIdx, int endIdx, float min, float max);
	std::vector<int>* findPath(const Urho3D::Vector3& from, const Urho3D::Vector2& aim);
	std::vector<int>* findPath(int startIdx, const Urho3D::Vector2& aim);
	float cost(int current, int next) const;
	void refreshWayOut(std::vector<int>& toRefresh);

	void invalidateCache();
	void debug(int start, int end);
	void drawMap(Urho3D::Image* image);
	void prepareGridToFind();
private:
	float heuristic(int from, int to) const;

	bool ifInCache(int startIdx, int end) const { return lastStartIdx == startIdx && lastEndIdx == end; }
	int getIndex(short posX, short posZ) const { return posX * resolution + posZ; }
	Urho3D::IntVector2 getCords(int index) const { return Urho3D::IntVector2(index / resolution, index % resolution); }
	void resetPathArrays();
	short getIndex(float value) const;

	std::vector<int>* tempPath;

	int lastStartIdx = -1;
	int lastEndIdx = -1;

	void updateCost(int startIdx, float x);

	const short resolution, halfResolution;
	const float fieldSize;
	bool pathInited = false;

	int* came_from;
	float* cost_so_far;
	ComplexBucketData* complexData;

	float invFieldSize;
	int staticCounter = 0;
	int min_cost_to_ref = 0;
	int max_cost_to_ref = resolution * resolution - 1;

	std::vector<short> closeIndex;

	BucketQueue frontier;
};
