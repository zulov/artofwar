#pragma once

#define QUEUE_BUCKETS_SIZE 10

#include "fiboheap.h"

class BucketQueue
{
public:
	BucketQueue();
	~BucketQueue();
	bool empty() const;
	void put(int item, float priority);
	int get();
	void init(float _max, float _min);
private:
	int getIndex(float priority) const;
	void updateCurrentIndex();
	float max;
	float min;
	float perBucket;
	int size;
	char currentIndex;

	FibHeap buckets[QUEUE_BUCKETS_SIZE];
};
