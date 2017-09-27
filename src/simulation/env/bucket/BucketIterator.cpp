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
		currentContent = bucketGrid->getContentAt((*levels)[index].first + dX, (*levels)[index].second + dZ);
		sizeContent = currentContent->size();
		secondIndex = 0;
	}

	Unit* entity = (*currentContent)[secondIndex];
	++secondIndex;
	return entity;

}

void BucketIterator::init(std::vector<std::pair<char, char>>* _levels, short _dX, short _dZ,
                          BucketGrid* _bucketGrid) {
	levels = _levels;
	dX = _dX;
	dZ = _dZ;
	index = 0;
	secondIndex = 0;
	bucketGrid = _bucketGrid;
	currentContent = bucketGrid->getContentAt((*levels)[index].first + dX, (*levels)[index].second + dZ);
	sizeContent = currentContent->size();
	levelSize = levels->size();
}
