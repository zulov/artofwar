#include "Benchmark.h"
#include <ctime>
#include <string>


Benchmark::Benchmark() {
	index = 0;
	sum = 0;
	avg = 0;
	time_t now = time(0);
	char* dt = ctime(&now);
	tm* ltm = localtime(&now);
	std::string name = "result/" + std::to_string(1900 + ltm->tm_year) + "" + std::to_string(1 + ltm->tm_mon) + "" +
		std::to_string(ltm->tm_mday) + "" + std::to_string(1 + ltm->tm_hour) + "" +
		std::to_string(1 + ltm->tm_min) + "" + std::to_string(+ ltm->tm_sec) + "performance.txt";

	output.open(name);
}


Benchmark::~Benchmark() {
	output.close();
}

double Benchmark::getAverageFPS() {
	return avg;
}

double Benchmark::getLastFPS() {
	return data[index];
}

void Benchmark::add(double fps) {
	index++;
	if (index >= LENGTH) {
		avg = sum / LENGTH;
		index = 0;
		sum = 0;
		save();
		loops++;
	}

	data[index] = fps;
	sum += fps;
}

void Benchmark::save() {
	for (int count = 0; count < LENGTH; count++) {
		output << data[count] << std::endl;
	}
}

long Benchmark::getLoops() {
	return loops;
}
