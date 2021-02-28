#include "InfluenceMapQuad.h"

#include <array>
#include <Urho3D/Resource/Image.h>
#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "objects/Physical.h"
#include "simulation/env/GridCalculator.h"
#include "simulation/env/GridCalculatorProvider.h"

InfluenceMapQuad::InfluenceMapQuad(int from, int to, const unsigned short size)
	: calculator(GridCalculatorProvider::get(pow(2, to), size)) {
	minRes = pow(2, from);
	maxRes = pow(2, to);
	dataSize = 0;
	for (int i = from; i <= to; ++i) {
		const int res = pow(2, i);
		dataSize += res * res;
	}
	data = new float[dataSize];
	float* ptr = data;
	for (int i = from; i <= to; ++i) {
		const int res = pow(2, i);
		const auto arraySize = res * res;
		maps.emplace_back(ptr, arraySize);
		ptr += arraySize;
	}
}

InfluenceMapQuad::~InfluenceMapQuad() {
	delete[] data;
}

void InfluenceMapQuad::ensureReady() {
	if (dataReady != false) {
		std::fill_n(data, dataSize - maps.back().size(), 0.f);
		for (int i = maps.size() - 2; i >= 0; --i) {
			auto parent = maps[i + 1];
			const auto parentRes = parent.size();
			const auto current = maps[i];
			const auto currentSize = current.size();
			for (int j = 0; j < parent.size(); ++j) {
				if (parent[j] > 0.f) {
					auto x = j / parentRes;
					auto z = j % parentRes;
					x /= 2;
					z /= 2;

					const int newIndex = x * currentSize + z;
					current[newIndex] += parent[j];
					assert(newIndex<currentSize);
					//calculator->getindexes test
				}
			}
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

unsigned short InfluenceMapQuad::getResolution() const {
	return calculator->getResolution();
}

void InfluenceMapQuad::update(Physical* thing, float value) {
	dataReady = false;
	maps.back()[calculator->indexFromPosition(thing->getPosition())] += value;
}

void InfluenceMapQuad::update(int index, float value) {
	dataReady = false;
	maps.back()[index] += value;
}

void InfluenceMapQuad::updateInt(Physical* thing, int value) {
	dataReady = false;
	maps.back()[calculator->indexFromPosition(thing->getPosition())] += value;
}

void InfluenceMapQuad::reset() {
	dataReady = false;
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
	for (const auto map : maps) {
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
