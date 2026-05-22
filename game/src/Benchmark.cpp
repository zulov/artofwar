#include "Benchmark.h"

#include <algorithm>
#include <ctime>
#include <numeric>
#include <string>

Benchmark::Benchmark() {
	if constexpr (BENCH_SAVE) {
		auto now = time(0);
		auto ltm = localtime(&now);
		std::string name = "result/" + std::to_string(1900 + ltm->tm_year) + "_" + std::to_string(1 + ltm->tm_mon) + "_" +
			std::to_string(ltm->tm_mday) + "_" + std::to_string(ltm->tm_hour) + "_" +
			std::to_string(ltm->tm_min) + "_" + std::to_string(ltm->tm_sec) + "_performance.txt";
		output.open(name);
	}
}


Benchmark::~Benchmark() {
	if constexpr (BENCH_SAVE) {
		output.close();
	}
}

void Benchmark::add(float fps, bool realFrame, unsigned char tickIndex) {
	data[index] = fps;
	++index;
	if (index >= BENCH_LENGTH) {
		index = 0;
		std::sort(data, data + BENCH_LENGTH);
		avgLowest = roundf(std::accumulate(std::begin(data), data + LOW_CUTOFF, 0.f) / LOW_CUTOFF);
		avgMiddle = roundf(std::accumulate(data + LOW_CUTOFF, data + HIGH_CUTOFF, 0.f) / (HIGH_CUTOFF - LOW_CUTOFF));
		avgHighest = roundf(std::accumulate(data + HIGH_CUTOFF, data + BENCH_LENGTH, 0.f) / (BENCH_LENGTH - HIGH_CUTOFF));
		save();
	}

	if (realFrame) {
		auto& idx = this->tickIndex[tickIndex];
		tickData[tickIndex][idx] = fps;
		++idx;
		if (idx >= TICK_SAMPLES) {
			idx = 0;
			tickAvg[tickIndex] = roundf(
				std::accumulate(tickData[tickIndex], tickData[tickIndex] + TICK_SAMPLES, 0.f) / TICK_SAMPLES);
		}
	}
}

void Benchmark::save() {
	if constexpr (BENCH_SAVE) {
		for (float count : data) {
			output << count << std::endl;
		}
	}
}
