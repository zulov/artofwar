#include "InfluenceMap.h"
#include "Game.h"
#include "debug/DebugLineRepo.h"
#include "simulation/env/Environment.h"
#include "colors/ColorPaletteRepo.h"


InfluenceMap::InfluenceMap(unsigned short resolution, float size,
                           float valueThresholdDebug): resolution(resolution),
                                                       fieldSize(size / resolution),
                                                       arraySize(resolution * resolution),
                                                       size(size),
                                                       calculator(resolution, size),
                                                       valueThresholdDebug(valueThresholdDebug) {
}

InfluenceMap::~InfluenceMap() = default;


void InfluenceMap::draw(short batch, short maxParts) {
	auto size = arraySize / maxParts;

	for (int i = batch * size; (i < arraySize && i < (batch + 1) * size); ++i) {
		drawCell(i, batch);
	}
}

void InfluenceMap::drawCell(int index, short batch) {
	Urho3D::Vector2 center2 = calculator.getCenter(index);
	Urho3D::Vector3 center = Game::getEnvironment()->getPosWithHeightAt(center2.x_, center2.y_);
	Urho3D::Color color = Game::getColorPaletteRepo()->getColor(getValueAt(index), valueThresholdDebug);
	DebugLineRepo::drawTriangle(DebugLineType::INFLUANCE,
	                            center + Urho3D::Vector3(-fieldSize / 3, 1, fieldSize / 3),
	                            center + Urho3D::Vector3(fieldSize / 3, 1, fieldSize / 3),
	                            center + Urho3D::Vector3(fieldSize / 3, 1, -fieldSize / 3),
	                            color, batch
	);
	DebugLineRepo::drawTriangle(DebugLineType::INFLUANCE,
	                            center + Urho3D::Vector3(fieldSize / 3, 1, -fieldSize / 3),
	                            center + Urho3D::Vector3(-fieldSize / 3, 1, -fieldSize / 3),
	                            center + Urho3D::Vector3(-fieldSize / 3, 1, fieldSize / 3),
	                            color, batch
	);
}

void InfluenceMap::print() const {
	for (int i = 0; i < resolution; ++i) {
		for (int j = 0; j < resolution; ++j) {
			std::cout << getValueAt(calculator.getIndex(i, j)) << " ";
		}
		std::cout << "\n";
	}
}
