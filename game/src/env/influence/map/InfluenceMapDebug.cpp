#include "InfluenceMap.h"

#include <algorithm>
#include <cmath>

#include <Urho3D/Resource/Image.h>

#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "debug/DebugLineRepo.h"
#include "env/Environment.h"

void InfluenceMap::drawRaw(short batch, short maxParts) {
	drawRaw(std::span<const float>(rawValues, arraySize), batch, maxParts);
}

void InfluenceMap::drawRaw(std::span<const float> values, short batch, short maxParts) {
	assert(values.size() == arraySize);
	auto size = arraySize / maxParts;
	for (int i = batch * size; i < arraySize && i < (batch + 1) * size; ++i) {
		drawCell(i, batch, values[i]);
	}
}

void InfluenceMap::drawKernel(short batch, short maxParts) {
	auto size = arraySize / maxParts;
	ensureReady();
	for (int i = batch * size; i < arraySize && i < (batch + 1) * size; ++i) {
		drawCell(i, batch, kernelValues[i]);
	}
}

Urho3D::Vector3 InfluenceMap::getVertex(const Urho3D::Vector2& center, Urho3D::Vector2 vertex) const {
	auto result = Game::getEnvironment()->getPosWithHeightAt(center.x_ + vertex.x_, center.y_ + vertex.y_);
	result.y_ += 1.f;
	return result;
}

void InfluenceMap::drawCell(int index, short batch, float value) const {
	const auto center = calculator->getCenter(index);
	const auto v = calculator->getFieldSize() / 2.3f;
	const auto a = getVertex(center, Urho3D::Vector2(-v, v));
	const auto b = getVertex(center, Urho3D::Vector2(v, -v));
	const auto c = getVertex(center, Urho3D::Vector2(v, v));
	const auto d = getVertex(center, Urho3D::Vector2(-v, -v));
	const auto color = Game::getColorPaletteRepo()->getColor(value, valueThresholdDebug);
	DebugLineRepo::drawTriangle(DebugLineType::INFLUENCE, a, c, b, color, batch);
	DebugLineRepo::drawTriangle(DebugLineType::INFLUENCE, b, d, a, color, batch);
}

void InfluenceMap::printMap(std::span<const float> map, const Urho3D::String& name) {
	auto [minIt, maxIt] = std::minmax_element(map.begin(), map.end());
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
}
