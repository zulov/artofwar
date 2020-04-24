#pragma once
#include <vector>

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
	~RandGen();
	static RandGen* instance;
	int indexesFloat[RAND_FLOAT_SIZE];
	int indexesInt[RAND_INT_SIZE];
	std::vector<float>* dataFloat[RAND_FLOAT_SIZE];
	std::vector<int>* dataInt[RAND_INT_SIZE];
};

