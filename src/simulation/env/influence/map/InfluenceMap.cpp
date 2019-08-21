#include "InfluenceMap.h"
#include "Game.h"
#include "debug/DebugLineRepo.h"
#include "simulation/env/Environment.h"
#include "colors/ColorPaletteRepo.h"


InfluenceMap::InfluenceMap(unsigned short resolution, float size, float valueThresholdDebug): resolution(resolution), fieldSize(size / resolution),
                                                                   arraySize(resolution * resolution), size(size),
                                                                   calculator(resolution, size),valueThresholdDebug(valueThresholdDebug) {
}

InfluenceMap::~InfluenceMap() = default;


void InfluenceMap::draw(short batch, short maxParts) {
	auto size = arraySize / maxParts;

	for (int i = batch * size; (i < arraySize && i < (batch + 1) * size); ++i) {
		drawCell(i, batch);
	}
}

void InfluenceMap::drawCell(int index, short batch) {
	Urho3D::Vector3 center = calculator.getCenter(index);
	center = Game::getEnvironment()->getPosWithHeightAt(center.x_, center.z_);
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
