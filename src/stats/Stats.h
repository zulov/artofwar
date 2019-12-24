#pragma once
#include <vector>

#define INPUT_STATS_SIZE 24
#define STATS_PER_PLAYER_SIZE 12

class Stats {
public:
	Stats();
	~Stats();
	int getScoreFor(short id) const;
	float* getInputFor(short id);
	void init();
private:
	void update(short id);
	void clear();
	float* input;
	std::vector<float*> statsPerPlayer;
	float weights[STATS_PER_PLAYER_SIZE];
};
