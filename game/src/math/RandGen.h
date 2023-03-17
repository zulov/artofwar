#pragma once
#include <magic_enum.hpp>
#include "RandType.h"

constexpr short RAND_TAB_SIZE = 1024 * 2;

enum class RandType : char;

class RandGen {
public:
	static void init(bool isRandom);
	static void dispose();
	static void reset(bool isRandom);
	static float nextRand(RandFloatType type, float max = 1.f);
	static int nextRand(RandIntType type, int max = 255.f);
private:
	RandGen() = default;
	~RandGen() = default;
	static void resetIndexes();

	static RandGen* instance;
	int indexesFloat[magic_enum::enum_count<RandFloatType>()];
	int indexesInt[magic_enum::enum_count<RandIntType>()];
	float dataFloat[RAND_TAB_SIZE];
	int dataInt[RAND_TAB_SIZE];
};

