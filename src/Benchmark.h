#pragma once

#include <fstream>

#define BENCH_LENGTH 200
#define BENCH_SAVE false

class Benchmark {
public:
	Benchmark();
	~Benchmark();

	void add(float fps);
	void save();

	float getAverageFPS() const { return avg; }
	float getLastFPS() const { return data[index]; }
	int getLoops() const { return loops; }
	float getAvgLowest() const { return avgLowest; }
	float getAvgMiddle() const { return avgMiddle; }
	float getAvgHighest() const { return avgHighest; }
private:
	float data[BENCH_LENGTH]{};
	short index = 0;
	float sum = 0;
	float avg = 0;
	float avgLowest = 0;
	float avgMiddle = 0;
	float avgHighest = 0;
	int loops = 0;
	std::ofstream output;
};
