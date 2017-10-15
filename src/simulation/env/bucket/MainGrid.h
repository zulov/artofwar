#pragma once
#include "Grid.h"
#include "ComplexBucketData.h"

class MainGrid : public Grid
{
public:
	MainGrid(short _resolution, double _size, bool _debugEnabled = false);
	~MainGrid();

	bool validateAdd(Static* object);
	void addStatic(Static* object);
	void removeStatic(Static* object);
	Vector3* validatePosition(Vector3* position);

};
