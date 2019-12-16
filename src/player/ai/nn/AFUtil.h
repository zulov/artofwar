#pragma once
#include <cstdlib>

double sigm(double x) {
	return 1 / (1 + exp(-x));
}

double tanh(double x) {
	return tanh(x);
}

double fsigm(double x) {
	return x / (1 + abs(x));
}
