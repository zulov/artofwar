#pragma once

#define QUEUE_BUCKETS_SIZE 10

#include <functional>
#include <vector>
#include <queue>
#include "fiboheap.h"

class BucketQueue
{
public:
	BucketQueue(double _max, double _min);
	~BucketQueue();
	bool empty();
	void put(int item, double priority);
	int get();

private:
	int getIndex(double priority);
	int nextCurrent(int currentIndex);
	double max;
	double min;
	double perBucket;
	int size;
	int currentIndex;
//	typedef std::pair<double, int> PQElement;
//	std::priority_queue<
//		PQElement,
//		std::vector<PQElement>,
//		std::greater<PQElement>>
	FibHeap buckets[QUEUE_BUCKETS_SIZE];
	int histogram[QUEUE_BUCKETS_SIZE] = {0};
};
