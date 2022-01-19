#include "RandGen.h"

#include <ctime>
#include <random>

#include "utils/OtherUtils.h"

constexpr short RAND_TAB_SIZE = 1024;

RandGen* RandGen::instance = nullptr;

void RandGen::init(bool isRandom) {
	if (instance == nullptr) {
		instance = new RandGen();
	}
	resetIndexes();

	std::mt19937 gen(isRandom ? time(0) : 2);

	std::uniform_real_distribution disFloat(0.0, 1.0);
	for (auto& vector : instance->dataFloat) {
		vector.clear();
		vector.reserve(RAND_TAB_SIZE);
		for (int j = 0; j < RAND_TAB_SIZE; ++j) {
			vector.push_back(disFloat(gen));
		}
	}

	std::uniform_int_distribution disInt(0, INT32_MAX);
	for (auto& vector : instance->dataInt) {
		vector.clear();
		vector.reserve(RAND_TAB_SIZE);
		for (int j = 0; j < RAND_TAB_SIZE; ++j) {
			vector.push_back(disInt(gen));
		}
	}
}

void RandGen::dispose() {
	delete instance;
}

void RandGen::reset(bool isRandom) {
	if (isRandom) {
		init(true);
	}else {
		resetIndexes();
	}
}

void RandGen::resetIndexes() {
	std::fill_n(instance->indexesFloat, magic_enum::enum_count<RandFloatType>(), 0);
	std::fill_n(instance->indexesInt, magic_enum::enum_count<RandIntType>(), 0);
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
