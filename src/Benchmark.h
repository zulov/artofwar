#pragma once

#include <fstream>
#include "defines.h"

class Benchmark {
public:
	Benchmark();
	~Benchmark();
	double getAverageFPS();
	double getLastFPS();
	void add(double fps);
	void save();
	long getLoops();
private:
	double data[BENCH_LENGTH];
	int index;
	double sum;
	double avg;
	std::ofstream output;
	long loops = 0;
};

