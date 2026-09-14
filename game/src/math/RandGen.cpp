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
	instance->seed = isRandom ? static_cast<unsigned>(time(nullptr)) : 2;
	generateData();
}

void RandGen::generateData() {
	std::mt19937 gen(instance->seed);

	std::uniform_real_distribution disFloat(0.0, 1.0);
	for (float& j : instance->dataFloat) {
		j = disFloat(gen);
	}

	std::uniform_int_distribution disInt(0, INT32_MAX);
	for (int& j : instance->dataInt) {
		j = disInt(gen);
	}
}

void RandGen::dispose() { delete instance; }

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
	const auto id = castC(type);
	instance->indexesFloat[id]++;
	instance->indexesFloat[id] = instance->indexesFloat[id] % RAND_TAB_SIZE;
	return instance->dataFloat[instance->indexesFloat[id]] * max;
}

int RandGen::nextRand(RandIntType type, int max) {
	if (max <= 0) {
		return 0; // guard against modulo-by-zero (and negative bounds)
	}
	const auto id = castC(type);
	instance->indexesInt[id]++;
	instance->indexesInt[id] = instance->indexesInt[id] % RAND_TAB_SIZE;
	return instance->dataInt[instance->indexesInt[id]] % max;
}

RandSaveData RandGen::saveState() {
	RandSaveData state;
	state.seed = instance->seed;
	state.floatIndexes.assign(instance->indexesFloat, instance->indexesFloat + magic_enum::enum_count<RandFloatType>());
	state.intIndexes.assign(instance->indexesInt, instance->indexesInt + magic_enum::enum_count<RandIntType>());
	return state;
}

void RandGen::loadState(const RandSaveData& state) {
	if (instance == nullptr) {
		instance = new RandGen();
	}
	resetIndexes();
	instance->seed = state.seed;
	generateData();

	if (state.floatIndexes.size() == magic_enum::enum_count<RandFloatType>()) {
		for (size_t i = 0; i < state.floatIndexes.size(); ++i) {
			if (state.floatIndexes[i] >= 0 && state.floatIndexes[i] < RAND_TAB_SIZE) {
				instance->indexesFloat[i] = state.floatIndexes[i];
			}
		}
	}
	if (state.intIndexes.size() == magic_enum::enum_count<RandIntType>()) {
		for (size_t i = 0; i < state.intIndexes.size(); ++i) {
			if (state.intIndexes[i] >= 0 && state.intIndexes[i] < RAND_TAB_SIZE) {
				instance->indexesInt[i] = state.intIndexes[i];
			}
		}
	}
}
