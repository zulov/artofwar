#include "InfluenceMapQuad.h"

#include <array>
#include "objects/Physical.h"
#include "simulation/env/GridCalculator.h"
#include "simulation/env/GridCalculatorProvider.h"
#include "utils/DeleteUtils.h"

InfluenceMapQuad::InfluenceMapQuad(int from, int to, const unsigned short size, float valueThresholdDebug)
	: calculator(GridCalculatorProvider::get(pow(2, to), size)) {
	minRes = pow(2, from);
	maxRes = pow(2, to);
	for (; from <= to; ++from) {
		int res = pow(2, from);
		maps.push_back(new float[res * res]);
	}
}

InfluenceMapQuad::~InfluenceMapQuad() {
	clear_vector(maps);
}

void InfluenceMapQuad::ensureReady() {
	if (dataReady != false) {
		auto parentRes = calculator->getResolution();
		for (int i = maps.size() - 2; i >= 0; --i) {
			auto* const parent = maps[i + 1];
			auto* const current = maps[i];
			const auto currentSize = parentRes / 2;
			for (int j = 0; j < parentRes * parentRes; ++j) {
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
	int size = minRes * minRes;
	int index = std::distance(maps[0], std::max_element(maps[0], maps[0] + size));

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

void InfluenceMapQuad::print(Urho3D::String name) {
	ensureReady();
	for (auto i = 0; i < maps.size(); ++i) {
		//maps.at(i)->print(name);
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
