#include "DebugUtils.h"

#include <iostream>

#include "StringUtils.h"
int counterX = 0;
int counterY = 0;

void COUNT_X() {
	++counterX;
}

void COUNT_Y() {
	++counterY;
}

void PRINT_X2Y() {
	if (counterX + counterY > 0) {
		float sum = 100.f / (counterX + counterY);
		std::cout << "X: " << counterX << " Y: " << counterY << "| X-Y " << asStringF(counterX * sum, 1) << "%-" <<
			asStringF(counterY * sum, 1) << "%" << std::endl;
	}
}

void PRINT() {
	std::cout << std::endl;
}
