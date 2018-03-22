#pragma once
#include "defines.h"
#include <fstream>

class Benchmark {
public:
	Benchmark();
	~Benchmark();
	float getAverageFPS() const;
	float getLastFPS();
	void add(float fps);
	void save();
	int getLoops() const;
private:
	float data[BENCH_LENGTH]{};
	int index;
	float sum;
	float avg;
	std::ofstream output;
	int loops = 0;
};

