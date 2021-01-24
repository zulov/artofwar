#pragma once

#include <fstream>

constexpr int BENCH_LENGTH = 100;
constexpr int PERCENT = 10;
constexpr int PERCENT2 = 90;

constexpr bool BENCH_SAVE = false;

class Benchmark {
public:
	Benchmark();
	~Benchmark();

	void add(float fps);
	void save();

	float getAverageFPS() const { return avg; }
	float getLastFPS() const { return lastFps; }
	float getAvgLowest() const { return avgLowest; }
	float getAvgMiddle() const { return avgMiddle; }
	float getAvgHighest() const { return avgHighest; }
private:
	float data[BENCH_LENGTH]{};
	short index = 0;
	float lastFps = 0;
	float avg = 0;
	float avgLowest = 0;
	float avgMiddle = 0;
	float avgHighest = 0;
	std::ofstream output;
};
