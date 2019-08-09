#include "InfluenceMapFloat.h"
#include "objects/Physical.h"
#include <algorithm>
#include "debug/DebugLineRepo.h"
#include "Game.h"
#include "simulation/env/Environment.h"
#include "colors/ColorPaletteRepo.h"

InfluenceMapFloat::
InfluenceMapFloat(unsigned short resolution, float size, float coef, char level): InfluenceMap(resolution, size),
                                                                                  coef(coef),
                                                                                  level(level) {
	values = new float[arraySize];
}

InfluenceMapFloat::~InfluenceMapFloat() {
	delete[] values;
}

void InfluenceMapFloat::update(Physical* physical) {
	auto iX = calculator.getIndex(physical->getPosition().x_);
	auto iZ = calculator.getIndex(physical->getPosition().z_);
	for (int i = iX - level; i < iX + level; ++i) {
		if (validIndex(i)) {
			for (int j = iZ - level; j < iZ + level; ++j) {
				if (validIndex(j)) {
					auto a = (i - iX) * (i - iX);
					auto b = (j - iZ) * (j - iZ);
					int index = calculator.getIndex(i, j);
					values[index] += 1 / ((a + b) * coef + 1);
				}
			}
		}
	}
}

void InfluenceMapFloat::reset() {
	std::fill_n(values, arraySize, 0);
}

bool InfluenceMapFloat::validIndex(int i) const {
	return i >= 0 && i < resolution;
}

float InfluenceMapFloat::getValueAt(int index) {
	return values[index];
}

