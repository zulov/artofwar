#include "InfluenceMap.h"
#include "Game.h"
#include "debug/DebugLineRepo.h"
#include "simulation/env/Environment.h"
#include "colors/ColorPaletteRepo.h"


InfluenceMap::InfluenceMap(unsigned short resolution, float size, float valueThresholdDebug):
	resolution(resolution), size(size), fieldSize(size / resolution),
	arraySize(resolution * resolution), valueThresholdDebug(valueThresholdDebug),
	calculator(resolution, size) {
}


void InfluenceMap::draw(short batch, short maxParts) const {
	auto size = arraySize / maxParts;

	for (int i = batch * size; i < arraySize && i < (batch + 1) * size; ++i) {
		drawCell(i, batch);
	}
}

void InfluenceMap::drawCell(int index, short batch) const {
	const auto center2 = calculator.getCenter(index);
	const auto center = Game::getEnvironment()->getPosWithHeightAt(center2.x_, center2.y_);
	const auto color = Game::getColorPaletteRepo()->getColor(getValueAt(index), valueThresholdDebug);

	DebugLineRepo::drawTriangle(DebugLineType::INFLUANCE,
	                            center + Urho3D::Vector3(-fieldSize / 3, 1, fieldSize / 3),
	                            center + Urho3D::Vector3(fieldSize / 3, 1, fieldSize / 3),
	                            center + Urho3D::Vector3(fieldSize / 3, 1, -fieldSize / 3),
	                            color, batch);
	DebugLineRepo::drawTriangle(DebugLineType::INFLUANCE,
	                            center + Urho3D::Vector3(fieldSize / 3, 1, -fieldSize / 3),
	                            center + Urho3D::Vector3(-fieldSize / 3, 1, -fieldSize / 3),
	                            center + Urho3D::Vector3(-fieldSize / 3, 1, fieldSize / 3),
	                            color, batch);
}

Urho3D::Vector2 InfluenceMap::getCenter(int index) const {
	return calculator.getCenter(index);
}

float InfluenceMap::getFieldSize() const {
	return calculator.getFieldSize();
}

void InfluenceMap::print() const {
	for (int i = 0; i < resolution; ++i) {
		for (int j = 0; j < resolution; ++j) {
			std::cout << getValueAt(calculator.getIndex(i, j)) << " ";
		}
		std::cout << "\n";
	}
}
