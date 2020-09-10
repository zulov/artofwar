#include "BucketQueue.h"


BucketQueue::BucketQueue() {
	FibHeap::initCache();
}

void BucketQueue::init(float _max, float _min) {
	max = _max;
	min = _min;
	size = 0;
	currentIndex = QUEUE_BUCKETS_SIZE - 1;
	perBucket = (max - min) / QUEUE_BUCKETS_SIZE;
	for (auto& bucket : buckets) {
		bucket.clear();
	}
}

bool BucketQueue::empty() const { return size == 0; }

void BucketQueue::put(int item, float priority) {
	const auto index = getIndex(priority);

	buckets[index].put(item, priority);

	if (index < currentIndex) {
		currentIndex = index;
	}
	++size;
}

int BucketQueue::get() {
	const auto best_item = buckets[currentIndex].get();

	if (buckets[currentIndex].empty()) {
		updateCurrentIndex();
	}
	--size;
	return best_item;
}

int BucketQueue::getIndex(float priority) const {
	if (priority >= max) {
		return QUEUE_BUCKETS_SIZE - 1;
	}
	if (priority <= min) {
		return 0;
	}
	return (priority - min) / perBucket;
}

void BucketQueue::updateCurrentIndex() {
	for (auto i = currentIndex + 1; i < QUEUE_BUCKETS_SIZE; ++i) {
		if (!buckets[i].empty()) {
			currentIndex = i;
			return;
		}
	}
	currentIndex = QUEUE_BUCKETS_SIZE - 1;
}
