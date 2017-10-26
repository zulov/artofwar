#include "BucketQueue.h"
#include <iostream>


BucketQueue::
BucketQueue(double _max, double _min): max(_max), min(_min), currentIndex(QUEUE_BUCKETS_SIZE - 1), size(0) {
	perBucket = (max - min) / QUEUE_BUCKETS_SIZE;
}

BucketQueue::~BucketQueue() {
	//		std::cout << std::endl;
	//		for (int i = 0; i < QUEUE_BUCKETS_SIZE; ++i) {
	//			std::cout << histogram[i] << " ";
	//		}
	//		int a = 5;
}

bool BucketQueue::empty() { return size == 0; }

void BucketQueue::put(int item, double priority) {
	int index = getIndex(priority);
	buckets[index].put(item, priority);

	//	if (histogram[index] < buckets[index].size()) {
	//		histogram[index] = buckets[index].size();
	//	}
	if (index <= currentIndex) {
		currentIndex = index;
	}
	++size;
}

int BucketQueue::get() {
	int best_item = buckets[currentIndex].get();

	if (buckets[currentIndex].empty()) {
		currentIndex = nextCurrent(currentIndex);
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

int BucketQueue::nextCurrent(int currentIndex) {
	for (int i = currentIndex + 1; i < QUEUE_BUCKETS_SIZE; ++i) {
		if (!buckets[i].empty()) {
			return i;
		}
	}
	return QUEUE_BUCKETS_SIZE - 1;
}
