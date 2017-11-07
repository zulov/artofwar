#pragma once

#define QUEUE_BUCKETS_SIZE 10

#include "fiboheap.h"

class BucketQueue
{
public:
	BucketQueue();
	~BucketQueue();
	bool empty();
	void put(int item, double priority);
	int get();
	void init(double _max, double _min);
private:
	int getIndex(double priority);
	void updateCurrentIndex();
	double max;
	double min;
	double perBucket;
	int size;
	int currentIndex;

	FibHeap buckets[QUEUE_BUCKETS_SIZE];
};
