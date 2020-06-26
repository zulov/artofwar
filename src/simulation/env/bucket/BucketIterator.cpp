#include "BucketIterator.h"
#include "Grid.h"

Physical* BucketIterator::next() {
	while (currentIterator == currentEnd) {
		++index;
		if (index >= levelSize) { return nullptr; }

		setRange();
	}

	return *(currentIterator++);
}

BucketIterator* BucketIterator::init(std::vector<short>* _levels, int _center, Grid* _bucketGrid) {
	levels = _levels;
	center = _center;
	index = 0;
	bucketGrid = _bucketGrid;
	setRange();
	levelSize = levels->size();
	return this;
}

void BucketIterator::setRange() {
	auto& content = bucketGrid->getContentAt((*levels)[index] + center);

	currentIterator = content.begin();
	currentEnd = content.end();
}
