#include "Benchmark.h"



Benchmark::Benchmark() {
	index = 0;
	sum = 0;
	avg = 0;
}


Benchmark::~Benchmark() {}

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
	} 

	data[index] = fps;
	sum += fps;
}

void Benchmark::save() {

}
