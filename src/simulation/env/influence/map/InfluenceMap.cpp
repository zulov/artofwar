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

void InfluenceMap::print(Urho3D::String name) {
	auto image = new Urho3D::Image(Game::getContext());
	image->SetSize(resolution, resolution, 4);
	const auto data = (uint32_t*)image->GetData();

	for (short y = 0; y != resolution; ++y) {
		for (short x = 0; x != resolution; ++x) {
			const int index = calculator.getNotSafeIndex(x, y);
			const int idR = calculator.getIndex(resolution - y - 1, x);
			auto percent = getValueAsPercent(index);
			const auto color = Game::getColorPaletteRepo()->getColor(getValueAsPercent(index), 1);
			image->SetPixel(x, y, color);
			//*(data + idR) = color;
		}
	}

	Urho3D::String prefix = Urho3D::String(counter) + "_";

	image->SaveBMP("result/images/influance/" + prefix + name + ".bmp");
	++counter;

	delete image;
}
