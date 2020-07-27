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
	// auto image = new Urho3D::Image(Game::getContext());
	// image->SetSize(resolution, resolution, 4);
	//
	// for (int i = 0; i < resolution; ++i) {
	// 	for (int j = 0; j < resolution; ++j) {
	// 		std::cout << getValueAt(calculator.getIndex(i, j)) << " ";
	// 		const int index = calculator.getIndex(x, y);
	// 		const int idR = calculator.getIndex(resolution - y - 1, x);
	// 		if (complexData[index].isUnit()) {
	// 			*(data + idR) = 0xFFFFFFFF;
	// 		} else {
	// 			*(data + idR) = 0xFF000000;
	// 		}
	// 	}
	// 	std::cout << "\n";
	// }
	//
	//
	// Urho3D::String prefix = Urho3D::String(staticCounter) + "_";
	// drawMap(image);
	// image->SaveBMP("result/images/" + prefix + "1_grid_map.bmp");
	// //draw_grid_from(came_from, image);
	// //image->SaveBMP("result/images/" + prefix + "2_grid_from.bmp");
	// draw_grid_cost(cost_so_far, image, resolution);
	// image->SaveBMP("result/images/" + prefix + "3_grid_cost.bmp");
	//
	// auto path = reconstruct_path(start, end, came_from);
	// draw_grid_path(path, image, resolution);
	//
	// image->SaveBMP("result/images/" + prefix + "4_grid_path.bmp");
	//
	// delete image;
	// staticCounter++;
}
