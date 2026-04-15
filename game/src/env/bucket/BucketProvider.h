#pragma once
#include <vector>

class Bucket;

class BucketProvider {
public:
	~BucketProvider();

	static Bucket* get(int sqResolution);
	static void release(Bucket* buckets, int sqResolution);
private:
	BucketProvider() = default;

	struct PoolEntry {
		Bucket* buckets;
		int sqResolution;
	};

	static std::vector<PoolEntry> pool;
};
