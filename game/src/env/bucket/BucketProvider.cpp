#include "BucketProvider.h"

#include "Bucket.h"

std::vector<BucketProvider::PoolEntry> BucketProvider::pool;

BucketProvider::~BucketProvider() {
	for (auto& entry : pool) {
		delete[] entry.buckets;
	}
	pool.clear();
}

Bucket* BucketProvider::get(int sqResolution) {
	for (auto it = pool.begin(); it != pool.end(); ++it) {
		if (it->sqResolution == sqResolution) {
			Bucket* buckets = it->buckets;
			pool.erase(it);
			return buckets;
		}
	}
	return new Bucket[sqResolution];
}

void BucketProvider::release(Bucket* buckets, int sqResolution) {
	for (int i = 0; i < sqResolution; ++i) {
		buckets[i].clear();
	}
	pool.push_back({buckets, sqResolution});
}
