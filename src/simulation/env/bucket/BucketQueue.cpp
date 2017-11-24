#include "BucketQueue.h"


BucketQueue::BucketQueue() {
}

BucketQueue::~BucketQueue() {
}

void BucketQueue::init(double _max, double _min) {
	max = _max;
	min = _min;
	size = 0;
	currentIndex = QUEUE_BUCKETS_SIZE - 1;
	perBucket = (max - min) / QUEUE_BUCKETS_SIZE;
	for (int i = 0; i < QUEUE_BUCKETS_SIZE; ++i) {
		buckets[i].clear();
	}
}

bool BucketQueue::empty() { return size == 0; }

void BucketQueue::put(int item, double priority) {
	int index = getIndex(priority);
	buckets[index].put(item, priority);

	//		if (histogram[index] < buckets[index].size()) {
	//			histogram[index] = buckets[index].size();
	//		}
	if (index < currentIndex) {
		currentIndex = index;
	}
	++size;
}

int BucketQueue::get() {
	int best_item = buckets[currentIndex].get();

	if (buckets[currentIndex].empty()) {
		updateCurrentIndex();
	}
	--size;
	return best_item;
}

int BucketQueue::getIndex(double priority) {
	if (priority >= max) {
		return QUEUE_BUCKETS_SIZE - 1;
	}
	if (priority <= min) {
		return 0;
	}
	return (priority - min) / perBucket;
}

void BucketQueue::updateCurrentIndex() {
	for (int i = currentIndex + 1; i < QUEUE_BUCKETS_SIZE; ++i) {
		if (!buckets[i].empty()) {
			currentIndex = i;
			break;
		}
	}
	currentIndex = QUEUE_BUCKETS_SIZE - 1;
}
