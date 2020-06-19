#include "NewBucketIterator.h"


#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"

template <class T>
T* NewBucketIterator<T>::next() {
	while (currentIterator == currentEnd) {
		++index;
		if (index >= levelSize) { return nullptr; }

		setRange();
	}

	const auto entity = *currentIterator;
	++currentIterator;
	return entity;
}

template <class T>
NewBucketIterator<T>* NewBucketIterator<T>::init(std::vector<short>* _levels, int _center, AbstractGrid<T>* _bucketGrid) {
	levels = _levels;
	center = _center;
	index = 0;
	bucketGrid = _bucketGrid;
	setRange();
	levelSize = levels->size();
	return this;
}

template <class T>
void NewBucketIterator<T>::setRange() {
	auto& content = bucketGrid->getContentAt((*levels)[index] + center);

	currentIterator = content.begin();
	currentEnd = content.end();
}

template class NewBucketIterator<ResourceEntity>;
template class NewBucketIterator<Building>;
