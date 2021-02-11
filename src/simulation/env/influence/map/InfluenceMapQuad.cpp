#include "InfluenceMapQuad.h"

#include <array>


#include "simulation/env/GridCalculatorProvider.h"
#include "simulation/env/influence/InfluenceMapFactory.h"
#include "utils/DeleteUtils.h"

InfluenceMapQuad::InfluenceMapQuad(int from, int to, InfluenceMapType type, const unsigned short size, float coef,
                                   float valueThresholdDebug): calculator(
	GridCalculatorProvider::get(pow(2, to), size)) {
	minSize = pow(2, from);
	maxSize = pow(2, to);
	for (; from <= to; ++from) {
		maps.push_back(new float[pow(2, from)]);
	}
}

InfluenceMapQuad::~InfluenceMapQuad() {
	clear_vector(maps);
}

void InfluenceMapQuad::ensureReady() {
	if (dataReady != false) {

		auto size = calculator->getResolution();
		for (int i = maps.size() - 2; i >= 0; --i) {
			auto d = maps[i - 1];
			auto lessSize = size / 2;
			if (d[i] > 0.f) {
				auto x = i / size;
				auto z = i & size;
				x /= 2;
				z /= 2;

				int newIndex = x * lessSize + z;
				maps[i][newIndex] += d[i];
				//calculator->getindexes test
			}
			size = lessSize;
		}
		for (int i = 0; i < size; ++i) { }
		dataReady = true;
	}
}

Urho3D::Vector2 InfluenceMapQuad::getCenter() {
	ensureReady();
	int index = std::distance(maps[0], std::max_element(maps[0], maps[0] + minSize));
	int size = minSize;
	for (int i = 1; i < maps.size(); ++i) {
		size *= 4;
		std::array<int, 4> indexes = getIndexes(size, index);
		index = getMaxElement(indexes, maps[i]);
	}
	return calculator->getCenter(index);
}

int InfluenceMapQuad::getMaxElement(const std::array<int, 4>& indexes, const float* values) const {
	float values1[4] = {values[indexes[0]], values[indexes[1]], values[indexes[2]], values[indexes[3]]};
	int i = std::distance(values1, std::max_element(values1, values1 + 4));
	return indexes[i];
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
	auto size = minSize;
	for (auto map : maps) {
		std::fill_n(map, size, 0.f);
		size *= 4;
	}
}

void InfluenceMapQuad::print(Urho3D::String name) {
	ensureReady();
	for (auto i = 0; i < maps.size(); ++i) {
		maps.at(i)->print(name);
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
