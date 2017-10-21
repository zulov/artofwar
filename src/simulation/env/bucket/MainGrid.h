#pragma once
#include "Grid.h"
#include "ComplexBucketData.h"

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
private:
	IntVector2 calculateSize(int size);

};
