#include "RandGen.h"
#include <random>

#include "utils/OtherUtils.h"


constexpr short RAND_TAB_SIZE = 1000;

RandGen* RandGen::instance = nullptr;

void RandGen::init() {
	if (instance == nullptr) {
		instance = new RandGen();
	}
	{
		std::mt19937 gen(2);
		std::uniform_real_distribution disFloat(0.0, 1.0);
		std::fill_n(instance->indexesFloat, magic_enum::enum_count<RandFloatType>(), 0);
		for (int i = 0; i < magic_enum::enum_count<RandFloatType>(); ++i) {
			instance->dataFloat[i].reserve(RAND_TAB_SIZE);
			for (int j = 0; j < RAND_TAB_SIZE; ++j) {
				instance->dataFloat[i].push_back(disFloat(gen));
			}
		}
	}
	{
		std::mt19937 gen(2);
		std::uniform_int_distribution disInt(0, INT32_MAX);
		std::fill_n(instance->indexesInt, magic_enum::enum_count<RandIntType>(), 0);
		for (int i = 0; i < magic_enum::enum_count<RandIntType>(); ++i) {
			instance->dataInt[i].reserve(RAND_TAB_SIZE);
			for (int j = 0; j < RAND_TAB_SIZE; ++j) {
				instance->dataInt[i].push_back(disInt(gen));
			}
		}
	}
}

void RandGen::dispose() {
	delete instance;
}

float RandGen::nextRand(RandFloatType type, float max) {
	auto id = cast(type);
	instance->indexesFloat[id]++;
	instance->indexesFloat[id] = instance->indexesFloat[id] % RAND_TAB_SIZE;
	return instance->dataFloat[id].at(instance->indexesFloat[id]) * max;
}

int RandGen::nextRand(RandIntType type, int max) {
	auto id = cast(type);
	instance->indexesInt[id]++;
	instance->indexesInt[id] = instance->indexesInt[id] % RAND_TAB_SIZE;
	return instance->dataInt[id].at(instance->indexesInt[id]) % max;
}
