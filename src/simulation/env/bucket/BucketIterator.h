#pragma once
#include <vector>


class Grid;
class Unit;

class BucketIterator
{
public:
	BucketIterator();
	~BucketIterator();
	Unit* next();
	void setRange();
	void init(std::vector<short>* _levels, int _center, Grid* _bucketGrid);
private:
	short index = 0;

	short levelSize;
	std::vector<short>* levels;
	int center;
	std::vector<Unit *>::iterator currentIterator;
	std::vector<Unit *>::iterator currentEnd;
	Grid* bucketGrid;
};
