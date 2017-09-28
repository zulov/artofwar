#include "BucketIterator.h"
#include "BucketGrid.h"

BucketIterator::BucketIterator() {

}

BucketIterator::~BucketIterator() {
}

Unit* BucketIterator::next() {
	while (secondIndex == sizeContent) {
		if (index + 1 >= levelSize) { return nullptr; }
		++index;
		currentContent = bucketGrid->getContentAt((*levels)[index] + center);
		sizeContent = currentContent->size();
		secondIndex = 0;
	}

	Unit* entity = (*currentContent)[secondIndex];
	++secondIndex;
	return entity;

}

void BucketIterator::init(std::vector<int>* _levels, int _center, BucketGrid* _bucketGrid) {
	levels = _levels;
	center = _center;
	index = 0;
	secondIndex = 0;
	bucketGrid = _bucketGrid;
	currentContent = bucketGrid->getContentAt((*levels)[index] + center);
	sizeContent = currentContent->size();
	levelSize = levels->size();
}
