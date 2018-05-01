#include "BucketIterator.h"
#include "Grid.h"

BucketIterator::BucketIterator() = default;

BucketIterator::~BucketIterator() = default;

Unit* BucketIterator::next() {
	while (currentIterator == currentEnd) {
		++index;
		if (index >= levelSize) { return nullptr; }

		currentIterator = bucketGrid->getContentAt((*levels)[index] + center).begin();
		currentEnd = bucketGrid->getContentAt((*levels)[index] + center).end();
	}

	Unit* entity = *currentIterator;
	++currentIterator;
	return entity;
}

void BucketIterator::init(std::vector<short>* _levels, int _center, Grid* _bucketGrid) {
	levels = _levels;
	center = _center;
	index = 0;
	bucketGrid = _bucketGrid;
	currentIterator = bucketGrid->getContentAt((*levels)[index] + center).begin();
	currentEnd = bucketGrid->getContentAt((*levels)[index] + center).end();

	levelSize = levels->size();
}
