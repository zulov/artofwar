#pragma once
#include <vector>
#include <magic_enum.hpp>
#include "RandType.h"

enum class RandType : char;

class RandGen {
public:
	static void init();
	static void dispose();
	static float nextRand(RandFloatType type, float max = 1.f);
	static int nextRand(RandIntType type, int max = 255.f);
private:
	RandGen() = default;
	~RandGen() = default;
	static RandGen* instance;
	int indexesFloat[magic_enum::enum_count<RandFloatType>()];
	int indexesInt[magic_enum::enum_count<RandIntType>()];
	std::vector<float> dataFloat[magic_enum::enum_count<RandFloatType>()];
	std::vector<int> dataInt[magic_enum::enum_count<RandIntType>()];
};

