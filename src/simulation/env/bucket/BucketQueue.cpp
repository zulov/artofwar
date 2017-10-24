#include "BucketQueue.h"
#include <iostream>


BucketQueue::BucketQueue(double _max, double _min) {
	max = _max;
	min = _min;
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
	buckets[index].emplace(priority, item);

//	if (histogram[index] < buckets[index].size()) {
//		histogram[index] = buckets[index].size();
//	}
	if (index <= currentIndex) {
		currentIndex = index;
	}
	++size;
}

int BucketQueue::get() {
	int best_item = buckets[currentIndex].top().second;
	buckets[currentIndex].pop();
	if (buckets[currentIndex].empty()) {
		currentIndex = nextCurrent(currentIndex);
	}
	--size;
	return best_item;
}

int BucketQueue::getIndex(double priority) {
	if (priority >= max) {
		return QUEUE_BUCKETS_SIZE - 1;
	} else if (priority < min) {
		return 0;
	}
	return (priority-min) / perBucket;
}

int BucketQueue::nextCurrent(int currentIndex) {
	for (int i = currentIndex + 1; i < QUEUE_BUCKETS_SIZE; ++i) {
		if (!buckets[currentIndex].empty()) {
			return i;
		}
	}
}
