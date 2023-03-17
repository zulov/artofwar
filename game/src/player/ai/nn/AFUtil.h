#pragma once
#include <cstdlib>

inline  float sigm(float x) {
	return 1 / (1 + exp(-x));
}

inline float tanh1(float x) {
	return tanh(x);
}

inline float fsigm(float x) {
	return x / (1 + abs(x));
}
