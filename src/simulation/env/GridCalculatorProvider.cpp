#include "GridCalculatorProvider.h"
#include "utils/DeleteUtils.h"

std::vector<GridCalculator*> GridCalculatorProvider::calculators;

GridCalculatorProvider::~GridCalculatorProvider() {
	clear_vector(calculators);
}

GridCalculator* GridCalculatorProvider::get(unsigned short resolution, float size) {
	for (auto calculator : calculators) {
		if (calculator->getResolution() == resolution && calculator->getSize() == size) {
			return calculator;
		}
	}
	auto* const calculator = new GridCalculator(resolution, size);
	calculators.push_back(calculator);
	return calculator;
}
