#pragma once

#include <fstream>
#include "simulation/SimGlobals.h"

constexpr unsigned char BENCH_LENGTH = 120;
constexpr unsigned char LOW_CUTOFF = 12;
constexpr unsigned char HIGH_CUTOFF = 108;
constexpr unsigned char TICK_SAMPLES = 10;

constexpr bool BENCH_SAVE = false;

class Benchmark {
public:
	Benchmark();
	~Benchmark();

	void add(float fps, bool realFrame, unsigned char tickIndex);
	void save();

	float getAvgLowest() const { return avgLowest; }
	float getAvgMiddle() const { return avgMiddle; }
	float getAvgHighest() const { return avgHighest; }
	const float* getTickAvgs() const { return tickAvg; }
private:
	float data[BENCH_LENGTH]{};
	float avgLowest = 0;
	float avgMiddle = 0;
	float avgHighest = 0;

	float tickData[FRAMES_IN_PERIOD][TICK_SAMPLES]{};
	short tickIndex[FRAMES_IN_PERIOD]{};
	float tickAvg[FRAMES_IN_PERIOD]{};

	std::ofstream output;
	short index = 0;
};
