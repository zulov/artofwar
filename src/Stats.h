#pragma once
#include <vector>

#define INPUT_STATS_SIZE 24


class Stats {
public:
	~Stats();
	int getScoreFor(short id) const;
	float* getInputFor(short id);
	void init();
private:
	void clear();
	std::vector<float *> inputs;
};
