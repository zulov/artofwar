#pragma once
#include <vector>
#include "GridCalculator.h"

class GridCalculatorProvider {
public:
	~GridCalculatorProvider();
	
	static GridCalculator* get(unsigned short resolution, float size);
private:
	GridCalculatorProvider() = default;
	static std::vector<GridCalculator*> calculators;
};
