#include "InfluenceMap.h"

#include <algorithm>
#include <cmath>

#include <Urho3D/Resource/Image.h>

#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "debug/DebugLineRepo.h"

void InfluenceMap::drawRaw() {
	DebugLineRepo::drawQuads(DebugLineType::GRID, getResolution(), rawValues, valueThresholdDebug);
}

void InfluenceMap::drawKernel() {
	ensureKernel();
	DebugLineRepo::drawQuads(DebugLineType::GRID, getResolution(), kernelValues, valueThresholdDebug);
}

void InfluenceMap::printMap(std::span<const float> map, const Urho3D::String& name) {
	auto [minIt, maxIt] = std::ranges::minmax_element(map);
	const auto minV = *minIt;
	const auto maxV = *maxIt;
	const float diff = maxV - minV;
	if (diff == 0.f) { return; }

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
	ensureKernel();
	printMap(std::span<const float>(rawValues, arraySize), name + "_raw");
	printMap(std::span<const float>(kernelValues, arraySize), name + "_kernel");
	ensureCenter();
	for (int i = 0; i < static_cast<int>(quadLayers.size()); ++i) {
		printMap(quadLayers[i], name + "_quad_" + Urho3D::String(i));
	}
}
