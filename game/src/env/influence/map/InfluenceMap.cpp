#include "InfluenceMap.h"

#include <algorithm>
#include <iterator>
#include <Urho3D/Resource/Image.h>
#include <vector>

#include "Game.h"
#include "debug/DebugLineRepo.h"
#include "env/Environment.h"
#include "colors/ColorPaletteRepo.h"
#include "env/GridCalculatorProvider.h"


InfluenceMap::InfluenceMap(unsigned short resolution, float size, float valueThresholdDebug):
	arraySize(resolution * resolution), valueThresholdDebug(valueThresholdDebug),
	calculator(GridCalculatorProvider::get(resolution, size)) {}


void InfluenceMap::draw(short batch, short maxParts) {
	auto size = arraySize / maxParts;
	ensureReady();
	for (int i = batch * size; i < arraySize && i < (batch + 1) * size; ++i) {
		drawCell(i, batch);
	}
}

Urho3D::Vector3 InfluenceMap::getVertex(const Urho3D::Vector2 &center, Urho3D::Vector2 vertex) const {
	auto result = Game::getEnvironment()->getPosWithHeightAt(center.x_ + vertex.x_, center.y_ + vertex.y_);
	result.y_ += 1.f;
	return result;
}

void InfluenceMap::drawCell(int index, short batch) const {
	const auto center = calculator->getCenter(index);

	const auto v = calculator->getFieldSize() / 2.3f;

	const auto a = getVertex(center, Urho3D::Vector2(-v, v));
	const auto b = getVertex(center, Urho3D::Vector2(v, -v));
	const auto c = getVertex(center, Urho3D::Vector2(v, v));
	const auto d = getVertex(center, Urho3D::Vector2(-v, -v));

	const auto color = Game::getColorPaletteRepo()->getColor(getValueAt(index), valueThresholdDebug);

	DebugLineRepo::drawTriangle(DebugLineType::INFLUENCE, a, c, b, color, batch);
	DebugLineRepo::drawTriangle(DebugLineType::INFLUENCE, b, d, a, color, batch);
}

void InfluenceMap::print(Urho3D::String name) {
	auto image = new Urho3D::Image(Game::getContext());
	const auto resolution = getResolution();
	image->SetSize(resolution, resolution, 4);
	ensureReady();
	computeMinMax();
	for (unsigned short x = 0; x != resolution; ++x) {
		const int index = calculator->getNotSafeIndex(x, 0);
		for (short y = 0; y != resolution; ++y) {
			const auto color = Game::getColorPaletteRepo()->getSolidColor(getValueAsPercent(index + y), 1.f);
			image->SetPixel(x, y, color);
		}
	}
	image->Resize(256, 256);
	image->FlipVertical();
	image->SavePNG("result/images/infl/" + name + "_" + Urho3D::String(resolution) + "x" + Urho3D::String(resolution)
	               + "#" + Urho3D::String(counter) + ".png");
	++counter;

	delete image;
}

void InfluenceMap::computeMinMax() const {
	if (!minMaxInited) {
		std::vector<float> data(arraySize);
		for (unsigned i = 0; i < arraySize; ++i) {
			data[i] = getValueAt(i);
		}
		const auto [minPtr, maxPtr] = std::minmax_element(data.begin(), data.end());
		min = *minPtr;
		max = *maxPtr;
		minIdx = std::distance(data.begin(), minPtr);
		maxIdx = std::distance(data.begin(), maxPtr);
		minMaxInited = true;
	}
}

float InfluenceMap::getValueAsPercent(unsigned index) const {
	const float diff = max - min;
	if (diff != 0.f) {
		return (getValueAt(index) - min) / diff;
	}
	return 0.5f;
}
