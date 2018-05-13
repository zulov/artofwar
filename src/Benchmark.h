#pragma once

#include <fstream>

#define BENCH_LENGTH 200
#define BENCH_SAVE false

class Benchmark
{
public:
	Benchmark();
	~Benchmark();

	void add(float fps);
	void save();

	float getAverageFPS() const { return avg; }
	float getLastFPS() const { return data[index]; }
	int getLoops() const { return loops; }
private:
	float data[BENCH_LENGTH]{};
	short index;
	float sum;
	float avg;
	std::ofstream output;
	int loops = 0;
};
