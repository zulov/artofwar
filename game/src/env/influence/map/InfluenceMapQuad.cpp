#include "InfluenceMapQuad.h"

#include <array>
#include <Urho3D/Resource/Image.h>
#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "math/MathUtils.h"
#include "math/SpanUtils.h"
#include "objects/Physical.h"
#include "env/GridCalculator.h"
#include "env/GridCalculatorProvider.h"

InfluenceMapQuad::InfluenceMapQuad(unsigned short topResolution, float mapSize)
	: calculator(GridCalculatorProvider::get(topResolution, mapSize)) {
	dataSize = 0;
	auto currentRes = topResolution;
	while (currentRes % 2 == 0 && currentRes != 1) {
		dataSize += currentRes * currentRes;
		currentRes /= 2;
	}
	currentRes *= 2;
	data = new float[dataSize];
	float* ptr = data;
	for (int i = currentRes; i <= topResolution; i *= 2) {
		const auto arraySize = i * i;
		maps.emplace_back(ptr, arraySize);
		ptr += arraySize;
	}
	last = maps.back();
}

InfluenceMapQuad::~InfluenceMapQuad() {
	delete[] data;
}

void InfluenceMapQuad::ensureReady() {
	if (dataReady == false) {
		std::fill_n(data, dataSize - maps.back().size(), 0.f);
		auto sum = sumSpan(maps.back());
		if (sum != 0.f) {
			unsigned short parentRes = calculator->getResolution();
			unsigned short currentRes = parentRes / 2;
			for (int i = maps.size() - 2; i >= 0; --i, parentRes /= 2, currentRes /= 2) {
				auto parent = maps[i + 1];
				const auto current = maps[i];

				int j = 0;
				for (float parent1 : parent) {
					if (parent1 > 0.f) {
						const int newIndex = getCordsInLower(currentRes, parentRes, j);
						assert(newIndex<currentRes*currentRes);
						current[newIndex] += parent1;
					}
					++j;
				}
			}
		}
		dataReady = true;
	}
}

std::optional<Urho3D::Vector2> InfluenceMapQuad::getCenter() {
	ensureReady(); //TODO performce remember center
	bool hasData = std::ranges::any_of(maps[0], [](float v) { return v > 0.f; });
	if (hasData) {
		int index = std::distance(maps[0].begin(), std::ranges::max_element(maps[0]));
		unsigned short res = sqrt(maps[0].size());
		for (int i = 1; i < maps.size(); ++i) {
			std::array<int, 4> indexes = getCordsInHigher(res, index);
			index = getMaxElement(indexes, maps[i]);
			res *= 2;
		}
		return calculator->getCenter(index);
	}
	return {};
}

int InfluenceMapQuad::getMaxElement(const std::array<int, 4>& indexes, std::span<float> values) const {
	float values1[4] = {values[indexes[0]], values[indexes[1]], values[indexes[2]], values[indexes[3]]};
	int i = std::distance(values1, std::max_element(values1, values1 + 4));
	return indexes[i];
}

unsigned short InfluenceMapQuad::getResolution() const {
	return calculator->getResolution();
}

void InfluenceMapQuad::update(int index, float value) {
	dataReady = false;
	last[index] += value;
}

void InfluenceMapQuad::updateInt(Physical* thing, int value) {
	updateInt(calculator->indexFromPosition(thing->getPosition()), value);
}

void InfluenceMapQuad::updateInt(int index, int value) {
	dataReady = false;
	last[index] += value;
}

void InfluenceMapQuad::reset() {
	//dataReady = false;
	std::fill_n(maps.back().begin(), maps.back().size(), 0.f);
}

void InfluenceMapQuad::print(const Urho3D::String& name, std::span<float> map) {
	auto [minIdx, maxIdx] = std::minmax_element(map.begin(), map.end());
	const auto min = *minIdx;
	const auto max = *maxIdx;
	const float diff = max - min;
	if (diff == 0.f) {
		return;
	}
	auto image = new Urho3D::Image(Game::getContext());
	const int resolution = sqrt(map.size());

	image->SetSize(resolution, resolution, 4);

	for (short y = 0; y != resolution; ++y) {
		for (short x = 0; x != resolution; ++x) {
			const int index = x * resolution + y;

			const auto color = Game::getColorPaletteRepo()->getSolidColor((map[index] - min) / diff, 1.f);
			image->SetPixel(x, resolution - y - 1, color);
		}
	}
	image->Resize(256, 256);

	image->SavePNG("result/images/infl/" + name + "_" + Urho3D::String(resolution) + "x" + Urho3D::String(resolution)
		+ "#" + Urho3D::String(counter) + ".png");
	++counter;

	delete image;
}

void InfluenceMapQuad::print(const Urho3D::String name) {
	ensureReady();
	for (const auto map : maps) {
		print(name, map);
	}
}
