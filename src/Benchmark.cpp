#include "Benchmark.h"
#include <ctime>
#include <string>


Benchmark::Benchmark() {
	index = 0;
	sum = 0;
	avg = 0;
	time_t now = time(0);
	tm* ltm = localtime(&now);
	std::string name = "result/" + std::to_string(1900 + ltm->tm_year) + "" + std::to_string(1 + ltm->tm_mon) + "" +
		std::to_string(ltm->tm_mday) + "" + std::to_string(1 + ltm->tm_hour) + "" +
		std::to_string(1 + ltm->tm_min) + "" + std::to_string(+ ltm->tm_sec) + "performance.txt";
	if constexpr (BENCH_SAVE) {
		output.open(name);
	}
}


Benchmark::~Benchmark() {
	if constexpr (BENCH_SAVE) {
		output.close();
	}
}

float Benchmark::getAverageFPS() const {
	return avg;
}

float Benchmark::getLastFPS() const {
	return data[index];
}

void Benchmark::add(float fps) {
	++index;
	if (index >= BENCH_LENGTH) {
		avg = sum / BENCH_LENGTH;
		index = 0;
		sum = 0;
		save();
		++loops;
	}

	data[index] = fps;
	sum += fps;
}

void Benchmark::save() {
	if constexpr (BENCH_SAVE) {
		for (float count : data) {
			output << count << std::endl;
		}
	}
}

int Benchmark::getLoops() const {
	return loops;
}
