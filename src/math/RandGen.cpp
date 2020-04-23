#include "RandGen.h"

#include <random>
#include "RandType.h"
#include "utils.h"


#define RAND_TAB_SIZE 100

RandGen* RandGen::instance = nullptr;


RandGen::~RandGen() {
	clear_array(instance->data, RAND_TYPE_SIZE);
}

void RandGen::init() {
	if (instance == nullptr) {
		instance = new RandGen();
	}

	std::mt19937 gen(1);
	std::uniform_real_distribution<double> dis(0.0, 1.0);
	std::fill_n(instance->indexes, RAND_TYPE_SIZE, 0);
	for (int i = 0; i < RAND_TYPE_SIZE; ++i) {
		instance->data[i] = new std::vector<float>();
		for (int j = 0; j < RAND_TAB_SIZE; ++j) {
			instance->data[i]->push_back(dis(gen));
		}
	}
}

void RandGen::dispose() {
	delete instance;
}

float RandGen::nextRand(RandType type, float max) {
	auto id = static_cast<char>(type);
	instance->indexes[id]++;
	instance->indexes[id] = instance->indexes[id] % RAND_TAB_SIZE;
	return instance->data[id]->at(instance->indexes[id]) * max;
}
