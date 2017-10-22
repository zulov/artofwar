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
	void draw_grid(int field_width, unordered_map<int, double>* distances, unordered_map<int, int>* point_to,
	               vector<int>* path);
	vector<int> reconstruct_path(IntVector2& startV, IntVector2& goalV, unordered_map<int, int>& came_from);
	bool inSide(int x, int z);
	vector<int> *neighbors(const int current);
	double cost(const int current, const int next);
	void findPath(IntVector2 &startV, IntVector2 &goalV,
		unordered_map<int, int>& came_from,
		std::unordered_map<int, double>& cost_so_far);
private:
	IntVector2 calculateSize(int size);
	vector<int> *tempNeighbour;
};
