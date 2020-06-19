#pragma once
#include "AbstractGrid.h"
#include <vector>


template <class T>
class NewBucketIterator {
public:
	NewBucketIterator() = default;
	~NewBucketIterator() = default;
	T* next();
	void setRange();

	NewBucketIterator<T>* init(std::vector<short>* _levels, int _center, AbstractGrid<T>* _bucketGrid);

private:
	short index = 0;
	short levelSize;
	int center;

	std::vector<short>* levels;
	typename std::vector<T*>::iterator currentIterator;
	typename std::vector<T*>::iterator currentEnd;
	AbstractGrid<T>* bucketGrid;
};
