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
	void all(std::vector<Physical*>& vector) const;
private:
	std::vector<short>::const_iterator index;
	std::vector<short>::const_iterator levelSize;
	int center;

	std::vector<short>* levels;
	Grid* bucketGrid;
	std::vector<Physical*>::const_iterator currentIterator;
	std::vector<Physical*>::const_iterator currentEnd;
};
