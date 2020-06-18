#pragma once
#include <vector>
#include "NewGrid.h"


template <class T>
class NewBucketIterator {
public:
	NewBucketIterator() = default;
	~NewBucketIterator() = default;
	T* next();
	void setRange();

	NewBucketIterator<T>* init(std::vector<short>* _levels, int _center, NewGrid<T>* _bucketGrid);

private:
	short index = 0;
	short levelSize;
	int center;

	std::vector<short>* levels;
	std::vector<T*>::iterator currentIterator;
	std::vector<T*>::iterator currentEnd;
	NewGrid<T>* bucketGrid;
};
