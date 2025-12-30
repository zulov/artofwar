#include "CountUtils.h"

#include <iostream>
#include <map>

#include "StringUtils.h"

int Count::counterX;
int Count::counterY;
std::map<std::string, int> Count::counters;

void Count::x() {
	++counterX;
}

void Count::y() {
	++counterY;
}
void Count::inc(const std::string& name) {
	++counters[name];
}

void Count::print_x2y() {
	if (counterX + counterY > 0) {
		float sum = 100.f / (counterX + counterY);
		std::cout << "X: " << counterX << " Y: " << counterY << "| X-Y "
			<< asStringF(counterX * sum, 1) << "%-"
			<< asStringF(counterY * sum, 1) << "%" << std::endl;
	}
}
void Count::print_counters() {
	if(!counters.empty()) {
		std::cout << "Counters:" << std::endl;
		for (const auto& pair : counters) {
			std::cout << pair.first << ": " << pair.second << std::endl;
		}
		std::cout << std::endl;
	}
}