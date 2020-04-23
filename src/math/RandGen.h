#pragma once
#include <vector>

#include "RandType.h"

enum class RandType : char;

class RandGen {
public:
	static void init();
	static void dispose();
	static float nextRand(RandType type, float max = 0.f);
private:
	RandGen() = default;
	~RandGen();
	static RandGen* instance;
	int indexes[RAND_TYPE_SIZE];
	std::vector<float>* data[RAND_TYPE_SIZE];
};

