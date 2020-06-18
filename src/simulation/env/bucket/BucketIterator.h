#pragma once
#include <vector>

class Grid;
class Physical;

class BucketIterator {
public:
	BucketIterator() = default;
	~BucketIterator() = default;
	Physical* next();
	void setRange();
	BucketIterator* init(std::vector<short>* _levels, int _center, Grid* _bucketGrid);
private:
	short index = 0;
	short levelSize;
	int center;

	std::vector<short>* levels;
	Grid* bucketGrid;
	std::vector<Physical*>::iterator currentIterator;
	std::vector<Physical*>::iterator currentEnd;
};
