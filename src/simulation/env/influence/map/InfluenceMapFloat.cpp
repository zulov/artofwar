#include "InfluenceMapFloat.h"
#include "objects/Physical.h"
#include <algorithm>
#include "DebugLineRepo.h"
#include "Game.h"
#include "simulation/env/Environment.h"

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

void InfluenceMapFloat::drawCell(Urho3D::Vector3 center) {
	center = Game::getEnvironment()->getPosWithHeightAt(center.x_, center.z_);

	DebugLineRepo::drawTriangle(DebugLineType::INFLUANCE,
	                            center + Urho3D::Vector3(2, 1, 2),
	                            center + Urho3D::Vector3(2, 1, 0),
	                            center + Urho3D::Vector3(0, 1, 2)
	);
}

void InfluenceMapFloat::draw() {
	for (int i = 0; i < arraySize; ++i) {
		Urho3D::Vector3 center = calculator.getCenter(i);
		drawCell(center);
	}
}

bool InfluenceMapFloat::validIndex(int i) const {
	return i >= 0 && i < resolution;
}
