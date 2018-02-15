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
	bool validateAdd(const IntVector2& size, Vector3& pos);
	void addStatic(Static* object);
	void removeStatic(Static* object);
	Vector3* getDirectionFrom(Vector3* position);
	Vector3* getValidPosition(const IntVector2& size, Vector3* pos);
	IntVector2 getBucketCords(const IntVector2& size, Vector3* pos) const;
	inline double heuristic(int from, int to);
	IntVector2 getCords(int index);

	std::vector<int>* reconstruct_path(IntVector2& startV, IntVector2& goalV, const int came_from[]);
	std::vector<int>* reconstruct_path(int start, int goal, const int came_from[]);
	bool inSide(int x, int z);
	void updateNeighbors(int current);
	double cost(int current, int next);
	void debug(IntVector2& startV, IntVector2& goalV);

	std::vector<int>* findPath(IntVector2& startV, IntVector2& goalV);
	std::vector<int>* findPath(int startIdx, int endIdx, double min);
	std::vector<int>* findPath(int startIdx, const Vector3& aim);

	void refreshWayOut(std::vector<int>& toRefresh);
	void draw_grid_from(int* cameFrom, Image* image);
	void draw_grid_cost(const float* costSoFar, Image* image);
	void draw_grid_path(std::vector<int>* path, Image* image);
	void drawMap(Image* image);
	content_info* getContentInfo(const Vector2& from, const Vector2& to, bool checks[], int activePlayer);
	Vector2& getCenterAt(const IntVector2& cords);
	Vector2 getCenter(int index);

private:
	void updateInfo(int index, content_info* ci, bool* checks, int activePlayer);
	static IntVector2 calculateSize(int size);
	std::vector<std::pair<int, float>>* tempNeighbour;
	std::vector<std::pair<int, float>>* tempNeighbour2;

	int staticCounter = 0;
	content_info* ci;

	int* came_from;
	float* cost_so_far;
	BucketQueue frontier;
	bool pathInited = false;

	ComplexBucketData* complexData;
};
