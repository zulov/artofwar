#include "BucketIterator.h"
#include "BucketGrid.h"

BucketIterator::BucketIterator() {

}

BucketIterator::~BucketIterator() {
}

Physical* BucketIterator::next() {
	while (secondIndex == sizeContent) {
		if (index >= levelSize) { return nullptr; }
		std::pair<short, short>* pair = (*levels)[index];
		++index;
		currentContent = bucketGrid->getContentAt(pair->first + dX, pair->second + dZ);
		sizeContent = currentContent->size();
		secondIndex = 0;
	}

	Physical* entity = (*currentContent)[secondIndex];
	++secondIndex;
	return entity;

}

void BucketIterator::init(std::vector<std::pair<short, short>*>* _levels, short _dX, short _dZ, BucketGrid* _bucketGrid) {
	levels = _levels;
	dX = _dX;
	dZ = _dZ;
	index = 0;
	secondIndex = 0;
	std::pair<short, short>* pair = (*levels)[index];
	bucketGrid = _bucketGrid;
	currentContent = bucketGrid->getContentAt(pair->first + dX, pair->second + dZ);
	sizeContent = currentContent->size();
	levelSize = levels->size();
	++index;
}
