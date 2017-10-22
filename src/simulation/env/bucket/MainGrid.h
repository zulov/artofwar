#pragma once
#include "Grid.h"
#include "ComplexBucketData.h"
#include <unordered_map>

class MainGrid : public Grid
{
public:
	MainGrid(short _resolution, double _size, bool _debugEnabled = false);
	~MainGrid();

	bool validateAdd(Static* object);
	bool validateAdd(const IntVector2& size, Vector3* pos);
	void addStatic(Static* object);
	void removeStatic(Static* object);
	Vector3* getDirectionFrom(Vector3* position);
	Vector3* getValidPosition(const IntVector2& size, Vector3* pos);
	IntVector2 getBucketCords(const IntVector2& size, Vector3* pos);
	inline double heuristic(int from, int to);
	IntVector2 getCords(const int index);

	vector<int> reconstruct_path(IntVector2& startV, IntVector2& goalV, int came_from[]);
	bool inSide(int x, int z);
	vector<int>* neighbors(const int current);
	double cost(const int current, const int next);
	void findPath(IntVector2& startV, IntVector2& goalV,
	              int came_from[], double cost_so_far[]);
	void draw_grid_from(int* cameFrom);
	void draw_grid_cost(double* costSoFar);
	void draw_grid_path(vector<int>* path);
private:
	IntVector2 calculateSize(int size);
	vector<int>* tempNeighbour;
};
