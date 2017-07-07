#include "BucketIterator.h"
#include "BucketGrid.h"

BucketIterator::BucketIterator(std::vector<std::pair<short, short>*>* _levels, short _dX, short _dZ, BucketGrid* _bucketGrid) {
	levels = _levels;
	dX = _dX;
	dZ = _dZ;
	std::pair<short, short>* pair = (*levels)[index];
	bucketGrid = _bucketGrid;
	currentContent = bucketGrid->getBucketAt(pair->first + dX, pair->second + dZ)->getContent();
	sizeContent = currentContent->size();
	levelSize = levels->size();
	++index;
}

BucketIterator::~BucketIterator() {
}

Entity* BucketIterator::next() {
	while (secondIndex == sizeContent) {
		if (index >= levelSize) { return nullptr; }
		std::pair<short, short>* pair = (*levels)[index];
		++index;
		currentContent = bucketGrid->getBucketAt(pair->first + dX, pair->second + dZ)->getContent();
		sizeContent = currentContent->size();
		secondIndex = 0;
	}

	Entity* entity = (*currentContent)[secondIndex];
	++secondIndex;
	return entity;

}
