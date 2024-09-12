#include "DebugUtils.h"

#include <iostream>
#include <map>

#include "StringUtils.h"
int counterX = 0;
int counterY = 0;
std::map<std::string, int> counters;

void COUNT_X() {
	++counterX;
}

void COUNT_Y() {
	++counterY;
}
void COUNT(const std::string& s) {
	++counters[s];
}

void PRINT_X2Y() {
	if (counterX + counterY > 0) {
		float sum = 100.f / (counterX + counterY);
		std::cout << "X: " << counterX << " Y: " << counterY << "| X-Y "
			<< asStringF(counterX * sum, 1) << "%-"
			<< asStringF(counterY * sum, 1) << "%" << std::endl;
	}
}
void PRINT_COUNTERS() {
	if(counters.size()>0) {
		std::cout << "Counters:" << std::endl;
		for (const auto& pair : counters) {
			std::cout << pair.first << ": " << pair.second << std::endl;
		}
		std::cout << std::endl;
	}
}