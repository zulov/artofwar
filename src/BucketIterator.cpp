#include "BucketIterator.h"
#include "BucketGrid.h"

BucketIterator::BucketIterator(std::vector<std::pair<int, int>*>* _levels, int _dX, int _dZ, BucketGrid* _bucketGrid) {
	levels = _levels;
	dX = _dX;
	dZ = _dZ;
	std::pair<int, int>* pair = (*levels)[index];
	bucketGrid = _bucketGrid;
	currentContent = bucketGrid->getBucketAt(pair->first + dX, pair->second + dZ)->getContent();
	iterator = currentContent->begin();
	index++;
}

BucketIterator::~BucketIterator() {
}

Entity* BucketIterator::next() {
	while (iterator == currentContent->end()) {
		if (index >= levels->size()) { return nullptr; }
		std::pair<int, int>* pair = (*levels)[index];
		currentContent = bucketGrid->getBucketAt(pair->first + dX, pair->second + dZ)->getContent();
		iterator = currentContent->begin();
		index++;
	}

	Entity* entity;
	entity = *iterator;
	++iterator;
	return entity;

}
