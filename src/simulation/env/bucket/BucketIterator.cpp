#include "BucketIterator.h"
#include "Grid.h"

BucketIterator::BucketIterator() = default;

BucketIterator::~BucketIterator() = default;

Physical* BucketIterator::next() {
	while (currentIterator == currentEnd) {
		++index;
		if (index >= levelSize) { return nullptr; }

		setRange();
	}

	const auto entity = *currentIterator;
	++currentIterator;
	return entity;
}

void BucketIterator::init(std::vector<short>* _levels, int _center, Grid* _bucketGrid) {
	levels = _levels;
	center = _center;
	index = 0;
	bucketGrid = _bucketGrid;
	setRange();
	levelSize = levels->size();
}

void BucketIterator::setRange() {
	auto& content = bucketGrid->getContentAt((*levels)[index] + center);

	currentIterator = content.begin();
	currentEnd = content.end();
}
