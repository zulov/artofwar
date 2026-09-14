#pragma once
#include <magic_enum.hpp>
#include "RandType.h"
#include "scene/load/RuntimeSaveData.h"

constexpr short RAND_TAB_SIZE = 1024 * 2;

enum class RandType : char;

class RandGen {
public:
	static void init(bool isRandom);
	static void dispose();
	static void reset(bool isRandom);
	static float nextRand(RandFloatType type, float max = 1.f);
	static int nextRand(RandIntType type, int max = 255);
	static RandSaveData saveState();
	static void loadState(const RandSaveData& state);

private:
	RandGen() = default;
	~RandGen() = default;
	static void generateData();
	static void resetIndexes();

	static RandGen* instance;
	int indexesFloat[magic_enum::enum_count<RandFloatType>()];
	int indexesInt[magic_enum::enum_count<RandIntType>()];
	float dataFloat[RAND_TAB_SIZE];
	int dataInt[RAND_TAB_SIZE];
	unsigned seed = 2;
};
