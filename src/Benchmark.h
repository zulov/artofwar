#pragma once
#define LENGTH 2000
#include <fstream>


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
	double data[LENGTH];
	int index;
	double sum;
	double avg;
	std::ofstream output;
	long loops = 0;
};

