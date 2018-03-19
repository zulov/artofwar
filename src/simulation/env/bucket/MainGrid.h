#pragma once
#include "BucketQueue.h"
#include "ComplexBucketData.h"
#include "Grid.h"
#include "simulation/env/ContentInfo.h"


namespace Urho3D {
	class Image;
}

class MainGrid : public Grid
{
public:
	MainGrid(short _resolution, double _size, bool _debugEnabled = false);
	~MainGrid();

	void prepareGridToFind();
	bool validateAdd(Static* object);
	bool validateAdd(const IntVector2& size, Vector2& pos);
	void addStatic(Static* object);
	void removeStatic(Static* object);
	Vector2* getDirectionFrom(Vector3* position);
	Vector2 getValidPosition(const IntVector2& size, Vector2& pos);
	IntVector2 getBucketCords(const IntVector2& size, Vector2& pos) const;
	inline float heuristic(int from, int to);
	IntVector2 getCords(int index);

	std::vector<int>* reconstruct_path(IntVector2& startV, IntVector2& goalV, const int came_from[]);
	std::vector<int>* reconstruct_path(int start, int goal, const int came_from[]);
	std::vector<int>* reconstruct_simplify_path(int start, int goal, const int came_from[]);
	bool inSide(int x, int z);
	void updateNeighbors(int current);
	double cost(int current, int next);
	void debug(int start, int end);

	std::vector<int>* findPath(IntVector2& startV, IntVector2& goalV);
	std::vector<int>* findPath(int startIdx, int endIdx, double min, double max);
	std::vector<int>* findPath(int startIdx, const Vector2& aim);
	bool ifInCache(int startIdx, int end);

	void refreshWayOut(std::vector<int>& toRefresh);
	void drawMap(Image* image);
	content_info* getContentInfo(const Vector2& from, const Vector2& to, bool checks[], int activePlayer);
	Vector2& getCenterAt(const IntVector2& cords);
	Vector2& getCenter(int index);
	Vector2 repulseObstacle(Unit* unit);

	void invalidateCache();

private:
	void updateInfo(int index, content_info* ci, bool* checks, int activePlayer);
	static IntVector2 calculateSize(int size);
	std::vector<std::pair<int, float>>* tempNeighbour;
	std::vector<std::pair<int, float>>* tempNeighbour2;
	std::vector<int>* tempPath;

	int lastStartIdx = -1;
	int lastEndIdx = -1;

	int staticCounter = 0;
	content_info* ci;

	int* came_from;
	float* cost_so_far;
	int min_cost_to_ref = 0;
	int max_cost_to_ref = resolution * resolution - 1;
	void updateCost(int startIdx, float x);
	void resetPathArrays();
	BucketQueue frontier;
	bool pathInited = false;

	ComplexBucketData* complexData;
};
