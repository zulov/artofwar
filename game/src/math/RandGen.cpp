#include "RandGen.h"

#include <ctime>
#include <iostream>
#include <random>

#include "utils/OtherUtils.h"

RandGen* RandGen::instance = nullptr;

void RandGen::init(bool isRandom) {
	if (instance == nullptr) {
		instance = new RandGen();
	}
	resetIndexes();

	std::mt19937 gen(isRandom ? time(0) : 2);

	std::uniform_real_distribution disFloat(0.0, 1.0);
	for (float& j : instance->dataFloat) {
		j = disFloat(gen);
	}

	std::uniform_int_distribution disInt(0, INT32_MAX);
	for (int& j : instance->dataInt) {
		j = disInt(gen);
	}
}

void RandGen::dispose() {
	delete instance;
}

void RandGen::reset(bool isRandom) {
	if (isRandom) {
		init(true);
	} else {
		resetIndexes();
	}
}

void RandGen::resetIndexes() {
	std::fill_n(instance->indexesFloat, magic_enum::enum_count<RandFloatType>(), 0);
	std::fill_n(instance->indexesInt, magic_enum::enum_count<RandIntType>(), 0);
}

float RandGen::nextRand(RandFloatType type, float max) {
	const auto id = cast(type);
	instance->indexesFloat[id]++;
	instance->indexesFloat[id] = instance->indexesFloat[id] % RAND_TAB_SIZE;
	return instance->dataFloat[instance->indexesFloat[id]] * max;
}

int RandGen::nextRand(RandIntType type, int max) {
	const auto id = cast(type);
	instance->indexesInt[id]++;
	instance->indexesInt[id] = instance->indexesInt[id] % RAND_TAB_SIZE;
	return instance->dataInt[instance->indexesInt[id]] % max;
}
