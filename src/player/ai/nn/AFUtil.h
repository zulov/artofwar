#pragma once
#include <cstdlib>

float sigm(float x) {
	return 1 / (1 + exp(-x));
}

float tanh1(float x) {
	return tanh(x);
}

float fsigm(float x) {
	return x / (1 + abs(x));
}
