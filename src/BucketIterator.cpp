#include "BucketIterator.h"
#include "BucketGrid.h"

BucketIterator::BucketIterator(std::vector<std::pair<int, int>*>* _levels, int _dX, int _dZ, BucketGrid* _bucketGrid) {
	levels = _levels;
	dX = _dX;
	dZ = _dZ;
	std::pair<int, int>* pair = (*levels)[index];
	bucketGrid = _bucketGrid;
	currentContent = bucketGrid->getBucketAt(pair->first + dX, pair->second + dZ)->getContent();
	sizeContent = currentContent->size();
	levelSize = levels->size();
	index++;
}

BucketIterator::~BucketIterator() {
}

Entity* BucketIterator::next() {

	while (secondIndex == sizeContent) {
		if (index >= levelSize) { return nullptr; }
		std::pair<int, int>* pair = (*levels)[index];
		++index;
		currentContent = bucketGrid->getBucketAt(pair->first + dX, pair->second + dZ)->getContent();
		sizeContent = currentContent->size();
		secondIndex = 0;
	}

	Entity* entity = (*currentContent)[secondIndex];
	++secondIndex;
	return entity;

}
