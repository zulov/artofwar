#pragma once

#include <fstream>

constexpr int BENCH_LENGTH = 120;
constexpr int PERCENT = 12;
constexpr int PERCENT2 = 108;

constexpr bool BENCH_SAVE = false;

class Benchmark {
public:
	Benchmark();
	~Benchmark();

	void add(float fps);
	void save();

	float getAvgLowest() const { return avgLowest; }
	float getAvgMiddle() const { return avgMiddle; }
	float getAvgHighest() const { return avgHighest; }
private:
	float data[BENCH_LENGTH]{};
	short index = 0;
	float avgLowest = 0;
	float avgMiddle = 0;
	float avgHighest = 0;
	std::ofstream output;
};
