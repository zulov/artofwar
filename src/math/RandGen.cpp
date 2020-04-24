#include "RandGen.h"

#include <random>
#include "RandType.h"
#include "utils.h"


#define RAND_TAB_SIZE 500

RandGen* RandGen::instance = nullptr;


RandGen::~RandGen() {
	clear_array(instance->dataFloat, RAND_FLOAT_SIZE);
	clear_array(instance->dataInt, RAND_INT_SIZE);
}

void RandGen::init() {
	if (instance == nullptr) {
		instance = new RandGen();
	}
	{
		std::mt19937 gen(1);
		std::uniform_real_distribution<double> disFloat(0.0, 1.0);
		std::fill_n(instance->indexesFloat, RAND_FLOAT_SIZE, 0);
		for (int i = 0; i < RAND_FLOAT_SIZE; ++i) {
			instance->dataFloat[i] = new std::vector<float>();
			for (int j = 0; j < RAND_TAB_SIZE; ++j) {
				instance->dataFloat[i]->push_back(disFloat(gen));
			}
		}
	}
	{
		std::mt19937 gen(1);
		std::uniform_int_distribution<int> disInt(0, INT64_MAX);
		std::fill_n(instance->indexesInt, RAND_INT_SIZE, 0);
		for (int i = 0; i < RAND_INT_SIZE; ++i) {
			instance->dataInt[i] = new std::vector<int>();
			for (int j = 0; j < RAND_TAB_SIZE; ++j) {
				instance->dataInt[i]->push_back(disInt(gen));
			}
		}
	}

}

void RandGen::dispose() {
	delete instance;
}

float RandGen::nextRand(RandFloatType type, float max) {
	auto id = static_cast<char>(type);
	instance->indexesFloat[id]++;
	instance->indexesFloat[id] = instance->indexesFloat[id] % RAND_TAB_SIZE;
	return instance->dataFloat[id]->at(instance->indexesFloat[id]) * max;
}

int RandGen::nextRand(RandIntType type, int max) {
	auto id = static_cast<char>(type);
	instance->indexesInt[id]++;
	instance->indexesInt[id] = instance->indexesInt[id] % RAND_TAB_SIZE;
	return instance->dataInt[id]->at(instance->indexesInt[id]) % max;
}
