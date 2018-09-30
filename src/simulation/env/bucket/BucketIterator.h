#pragma once
#include <vector>


class Grid;
class Physical;

class BucketIterator
{
public:
	BucketIterator();
	~BucketIterator();
	Physical* next();
	void setRange();
	BucketIterator* init(std::vector<short>* _levels, int _center, Grid* _bucketGrid);
private:
	short index = 0;

	short levelSize;
	std::vector<short>* levels;
	int center;
	std::vector<Physical *>::iterator currentIterator;
	std::vector<Physical *>::iterator currentEnd;
	Grid* bucketGrid;
};
