#pragma once

#include <fstream>

#define BENCH_LENGTH 200
#define BENCH_SAVE false

class Benchmark {
public:
	Benchmark();
	~Benchmark();
	float getAverageFPS() const;
	float getLastFPS() const;
	void add(float fps);
	void save();
	int getLoops() const;
private:
	float data[BENCH_LENGTH]{};
	short index;
	float sum;
	float avg;
	std::ofstream output;
	int loops = 0;
};

