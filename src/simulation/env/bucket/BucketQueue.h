#pragma once

#define QUEUE_BUCKETS_SIZE 100

#include <functional>
#include <vector>
#include <queue>

class BucketQueue
{
public:
	BucketQueue();
	~BucketQueue();
	bool empty();
	void put(int item, double priority);
	int get();

private:
	int getIndex(double priority);
	int nextCurrent(int currentIndex);
	double max = 3000;
	double perBucket = max / QUEUE_BUCKETS_SIZE;
	int size = 0;
	int currentIndex = 0;
	typedef std::pair<double, int> PQElement;
	std::priority_queue<
		PQElement,
		std::vector<PQElement>,
		std::greater<PQElement>>
	buckets[QUEUE_BUCKETS_SIZE];
	int histogram[QUEUE_BUCKETS_SIZE] = {0};
};
