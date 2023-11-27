#include "BucketIterator.h"
#include "Grid.h"
#include "levels/LevelCache.h"

Physical* BucketIterator::next() {
	while (currentIterator == currentEnd) {
		++index;
		if (index == levelSize) { return nullptr; }

		setRange();
	}

	return *(currentIterator++);
}

BucketIterator* BucketIterator::init(const std::vector<short>* _levels, int _center, Grid* _bucketGrid) {
	index = _levels->begin();//przefilrowac puste odrazu
	levelSize = _levels->end();
	center = _center;
	bucketGrid = _bucketGrid;
	setRange();
	return this;
}

void BucketIterator::setRange() {
	const auto& content = bucketGrid->getContentAt((*index) + center);
	//TODO bug mo¿e braæ z drugiego konca jezeli index jest niepoprawny

	currentIterator = content.begin();
	currentEnd = content.end();
}
