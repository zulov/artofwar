#include "InfluenceMap.h"

#include <algorithm>
#include <cmath>

#include <Urho3D/Resource/Image.h>

#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "debug/DebugLineRepo.h"
#include "env/Environment.h"

void InfluenceMap::drawRaw() {
	for (auto i = 0; i < arraySize; ++i) {
		drawCell(i, false);
	}
}

void InfluenceMap::drawKernel() {
	ensureReady();
	for (auto i = 0; i < arraySize; ++i) {
		drawCell(i, true);
	}
}

Urho3D::Vector3 InfluenceMap::getVertex(const Urho3D::Vector2& center, Urho3D::Vector2 vertex) const {
	auto result = Game::getEnvironment()->getPosWithHeightAt(center.x_ + vertex.x_, center.y_ + vertex.y_);
	result.y_ += 1.f;
	return result;
}

void InfluenceMap::drawCell(int index, bool useKernel) const {
	const auto& corners = Game::getEnvironment()->getDebugCellCorners(calculator->getResolution(), index);
	const auto color = Game::getColorPaletteRepo()->getColor(useKernel ? getKernel(index) : getRaw(index), valueThresholdDebug);
	DebugLineRepo::drawQuad(DebugLineType::INFLUENCE, corners, color);
}

void InfluenceMap::printMap(std::span<const float> map, const Urho3D::String& name) {
	auto [minIt, maxIt] = std::ranges::minmax_element(map);
	const auto minV = *minIt;
	const auto maxV = *maxIt;
	const float diff = maxV - minV;
	if (diff == 0.f) {
		return;
	}

	auto* image = new Urho3D::Image(Game::getContext());
	const int resolution = static_cast<int>(std::sqrt(map.size()));
	image->SetSize(resolution, resolution, 4);

	for (short y = 0; y != resolution; ++y) {
		for (short x = 0; x != resolution; ++x) {
			const int index = x * resolution + y;
			const auto color = Game::getColorPaletteRepo()->getSolidColor((map[index] - minV) / diff, 1.f);
			image->SetPixel(x, resolution - y - 1, color);
		}
	}

	image->Resize(256, 256);
	image->SavePNG("result/images/infl/" + name + "_" + Urho3D::String(resolution) + "x" + Urho3D::String(resolution)
	               + "#" + Urho3D::String(counter) + ".png");
	++counter;

	delete image;
}

void InfluenceMap::print(Urho3D::String name) {
	ensureReady();
	printMap(std::span<const float>(rawValues, arraySize), name + "_raw");
	printMap(std::span<const float>(kernelValues, arraySize), name + "_kernel");
	ensureQuad();
	for (int i = 0; i < static_cast<int>(quadLayers.size()); ++i) {
		printMap(quadLayers[i], name + "_quad_" + Urho3D::String(i));
	}
}
