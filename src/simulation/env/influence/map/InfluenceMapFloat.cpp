#include "InfluenceMapFloat.h"
#include "objects/Physical.h"
#include <algorithm>
#include "DebugLineRepo.h"
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

void InfluenceMapFloat::drawCell(int index) {
	Urho3D::Vector3 center = calculator.getCenter(index);
	center = Game::getEnvironment()->getPosWithHeightAt(center.x_, center.z_);
	Urho3D::Color color = Game::getColorPaletteRepo()->getColor(values[index], 10);
	DebugLineRepo::drawTriangle(DebugLineType::INFLUANCE,
	                            center + Urho3D::Vector3(fieldSize, 1, fieldSize),
	                            center + Urho3D::Vector3(fieldSize, 1, 0),
	                            center + Urho3D::Vector3(0, 1, fieldSize / 2),
	                            color
	);
}

void InfluenceMapFloat::draw() {
	for (int i = 0; i < arraySize; ++i) {
		drawCell(i);
	}
}

bool InfluenceMapFloat::validIndex(int i) const {
	return i >= 0 && i < resolution;
}
