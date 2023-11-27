#pragma once
#include <vector>

struct LevelCacheValue;
class Grid;
class Physical;

class BucketIterator {
public:
	BucketIterator() = default;
	~BucketIterator() = default;
	BucketIterator(const BucketIterator&) = delete;
	
	Physical* next();
	void setRange();
	BucketIterator* init(const std::vector<short>* _levels, int _center, Grid* _bucketGrid);
private:
	std::vector<short>::const_iterator index;
	std::vector<short>::const_iterator levelSize;
	int center;

	//std::vector<short>* levels;
	Grid* bucketGrid;
	std::vector<Physical*>::const_iterator currentIterator;
	std::vector<Physical*>::const_iterator currentEnd;
};
