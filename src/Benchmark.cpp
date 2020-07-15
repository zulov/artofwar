#include "Benchmark.h"

#include <algorithm>
#include <ctime>
#include <numeric>
#include <string>

#define PERCENT 10
#define PERCENT2 90

Benchmark::Benchmark() {
	if constexpr (BENCH_SAVE) {
		auto now = time(0);
		auto ltm = localtime(&now);
		std::string name = "result/" + std::to_string(1900 + ltm->tm_year) + "" + std::to_string(1 + ltm->tm_mon) + "" +
			std::to_string(ltm->tm_mday) + "" + std::to_string(1 + ltm->tm_hour) + "" +
			std::to_string(1 + ltm->tm_min) + "" + std::to_string(+ ltm->tm_sec) + "performance.txt";
		output.open(name);
	}
}


Benchmark::~Benchmark() {
	if constexpr (BENCH_SAVE) {
		output.close();
	}
}

void Benchmark::add(float fps) {
	if (index >= BENCH_LENGTH) {
		avg = sum / BENCH_LENGTH;
		index = 0;
		sum = 0;
		std::sort(data, data + BENCH_LENGTH);
		avgLowest = roundf(std::accumulate(std::begin(data), data + PERCENT, 0.f) / PERCENT);
		avgMiddle = roundf(std::accumulate(data + PERCENT, data + PERCENT2, 0.f) / (PERCENT2 - PERCENT));
		avgHighest = roundf(std::accumulate(data + PERCENT2, data + BENCH_LENGTH, 0.f) / (BENCH_LENGTH - PERCENT2));
		save();
		++loops;
	}
	data[index] = fps;
	sum += fps;
	++index;
}

void Benchmark::save() {
	if constexpr (BENCH_SAVE) {
		for (float count : data) {
			output << count << std::endl;
		}
	}
}
