#include "BucketIterator.h"
#include "Grid.h"

BucketIterator::BucketIterator(): currentContent() {
}

BucketIterator::~BucketIterator() = default;

Unit* BucketIterator::next() {
	while (secondIndex == sizeContent) {
		++index;
		if (index >= levelSize) { return nullptr; }

		currentContent = &bucketGrid->getContentAt((*levels)[index] + center);
		sizeContent = currentContent->size();
		secondIndex = 0;
	}

	Unit* entity = (*currentContent)[secondIndex];
	++secondIndex;
	return entity;
}

void BucketIterator::init(std::vector<short>* _levels, int _center, Grid* _bucketGrid) {
	levels = _levels;
	center = _center;
	index = 0;
	secondIndex = 0;
	bucketGrid = _bucketGrid;
	currentContent = &bucketGrid->getContentAt((*levels)[index] + center);
	sizeContent = currentContent->size();
	levelSize = levels->size();
}
