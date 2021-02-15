#include "InfluenceMapQuad.h"

#include <array>
#include <Urho3D/Resource/Image.h>
#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "objects/Physical.h"
#include "simulation/env/GridCalculator.h"
#include "simulation/env/GridCalculatorProvider.h"

InfluenceMapQuad::InfluenceMapQuad(int from, int to, const unsigned short size, float valueThresholdDebug)
	: calculator(GridCalculatorProvider::get(pow(2, to), size)) {
	minRes = pow(2, from);
	maxRes = pow(2, to);
	for (; from <= to; ++from) {
		const int res = pow(2, from);
		const auto arraySize = res * res;
		maps.push_back(std::span<float>(new float[size], arraySize));
	}
}

InfluenceMapQuad::~InfluenceMapQuad() {
	for (auto xes : maps) {
		delete[](xes.data());
	}
}

void InfluenceMapQuad::ensureReady() {
	if (dataReady != false) {
		auto parentRes = calculator->getResolution();
		for (int i = maps.size() - 2; i >= 0; --i) {
			auto parent = maps[i + 1];
			auto const current = maps[i];
			const auto currentSize = parentRes / 2;
			for (int j = 0; j < parent.size(); ++j) {
				if (parent[j] > 0.f) {
					auto x = j / parentRes;
					auto z = j % parentRes;
					x /= 2;
					z /= 2;

					const int newIndex = x * currentSize + z;
					current[newIndex] += parent[j];
					//calculator->getindexes test
				}
			}
			parentRes = currentSize;
		}

		dataReady = true;
	}
}

Urho3D::Vector2 InfluenceMapQuad::getCenter() {
	ensureReady();
	int index = std::distance(maps[0].begin(), std::max_element(maps[0].begin(), maps[0].end()));

	for (int i = 1; i < maps.size(); ++i) {
		std::array<int, 4> indexes = getIndexes(maps[i].size(), index);
		index = getMaxElement(indexes, maps[i]);
	}
	return calculator->getCenter(index);
}

int InfluenceMapQuad::getMaxElement(const std::array<int, 4>& indexes, std::span<float> values) const {
	float values1[4] = {values[indexes[0]], values[indexes[1]], values[indexes[2]], values[indexes[3]]};
	int i = std::distance(values1, std::max_element(values1, values1 + 4));
	return indexes[i];
}

unsigned short InfluenceMapQuad::getResolution() {
	return calculator->getResolution();
}

Urho3D::Vector2 InfluenceMapQuad::getCenter(int index) {
	return calculator->getCenter(index);
}

void InfluenceMapQuad::update(Physical* thing, float value) {
	dataReady = false;
	maps.back()[calculator->indexFromPosition(thing->getPosition())] += value;
}

void InfluenceMapQuad::updateInt(Physical* thing, int value) {
	dataReady = false;
	maps.back()[calculator->indexFromPosition(thing->getPosition())] += value;
}

void InfluenceMapQuad::reset() {
	dataReady = false;
	auto size = minRes * minRes;
	for (auto map : maps) {
		std::fill_n(map, size, 0.f);
		size *= 4;
	}
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
	const auto resolution = sqrt(map.size());	
	
	image->SetSize(resolution, resolution, 4);
	
	for (short y = 0; y != resolution; ++y) {
		for (short x = 0; x != resolution; ++x) {
			const int index = calculator->getNotSafeIndex(x, y);
			
			const auto color = Game::getColorPaletteRepo()->getColor((map[index] - min) / diff, 1.f);
			image->SetPixel(x, resolution - y - 1, color);
		}
	}
	image->Resize(256, 256);

	image->SavePNG("result/images/infl/" + name + "_" + Urho3D::String(resolution) + "x" + Urho3D::String(resolution)
		+ "#" + Urho3D::String(counter) + ".png");
	++counter;

	delete image;
}

void InfluenceMapQuad::print(Urho3D::String name) {
	ensureReady();
	for (auto map : maps) {
		print(name, map);
	}
}

std::array<int, 4> InfluenceMapQuad::getIndexes(unsigned short resolution, int index) const {
	const auto mod = index % resolution;
	const auto div = index / resolution;
	const auto value = mod * 2 + div * resolution * 4;

	return {
		value,
		value + 1,
		value + resolution * 2,
		value + resolution * 2 + 1
	};
}
