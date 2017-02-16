#pragma once
#define LENGTH 200


class Benchmark {
public:
	Benchmark();
	~Benchmark();
	double getAverageFPS();
	void add(double fps);
	void save();
private:
	double data[LENGTH];
	int index;
	double sum;
	double avg;

};

