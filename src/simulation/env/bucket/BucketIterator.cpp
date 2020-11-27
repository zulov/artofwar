#include "BucketIterator.h"
#include "Grid.h"

Physical* BucketIterator::next() {
	while (currentIterator == currentEnd) {
		++index;
		if (index == levelSize) { return nullptr; }

		setRange();
	}

	return *(currentIterator++);
}

BucketIterator* BucketIterator::init(std::vector<short>* _levels, int _center, Grid* _bucketGrid) {
	levels = _levels;
	index = levels->begin();
	levelSize = levels->end();
	center = _center;
	bucketGrid = _bucketGrid;
	setRange();
	return this;
}

void BucketIterator::all(std::vector<Physical*>* vector) const {
	for (auto level : *levels) {
		auto& content = bucketGrid->getContentAt(level + center);
		vector->insert(vector->end(), content.begin(), content.end());
	}
}

void BucketIterator::setRange() {
	auto& content = bucketGrid->getContentAt((*index) + center);

	currentIterator = content.begin();
	currentEnd = content.end();
}
