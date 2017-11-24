#pragma once
#include "Grid.h"
#include "ComplexBucketData.h"
#include <unordered_map>
#include "BucketQueue.h"
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
	bool validateAdd(const IntVector2& size, Vector3* pos);
	void addStatic(Static* object);
	void removeStatic(Static* object);
	Vector3* getDirectionFrom(Vector3* position);
	Vector3* getValidPosition(const IntVector2& size, Vector3* pos);
	IntVector2 getBucketCords(const IntVector2& size, Vector3* pos);
	inline double heuristic(int from, int to);
	IntVector2 getCords(const int index);

	std::vector<int> reconstruct_path(IntVector2& startV, IntVector2& goalV, int came_from[]);
	bool inSide(int x, int z);
	std::vector<std::pair<int, float>*>* neighbors(const int current);
	double cost(const int current, const int next);
	void debug(IntVector2& startV, IntVector2& goalV);
	void findPath(IntVector2& startV, IntVector2& goalV);
	void draw_grid_from(int* cameFrom, Image* image);
	void draw_grid_cost(float* costSoFar, Image* image);
	void draw_grid_path(std::vector<int>* path, Image* image);
	void drawMap(Image* image);
	content_info* getContentInfo(const Vector2& from, const Vector2& to, bool checks[], int activePlayer);
private:
	static IntVector2 calculateSize(int size);
	std::vector<std::pair<int, float>*>* tempNeighbour;
	int* came_from;
	float* cost_so_far;
	BucketQueue frontier;
	int staticCounter = 0;
	content_info* ci;

	ComplexBucketData *complexData;
};
