#include "InfluenceMapQuad.h"

#include "simulation/env/influence/InfluenceMapFactory.h"
#include "utils/DeleteUtils.h"

InfluenceMapQuad::InfluenceMapQuad(int from, int to, InfluenceMapType type, const unsigned short size, float coef,
                                   char level, float valueThresholdDebug) {
	for (; from <= to; ++from) {
		maps.push_back(createMap(pow(2, from), type, size, coef, level, valueThresholdDebug));
	}
}

InfluenceMapQuad::~InfluenceMapQuad() {
	clear_vector(maps);
}

Urho3D::Vector2 InfluenceMapQuad::getCenter() {
	int index = maps[0]->getMaxElement();
	for (int i = 1; i < maps.size(); ++i) {
		std::array<int, 4> indexes = getIndexes(maps[i]->getResolution(), index);
		index = maps[i]->getMaxElement(indexes);
	}
	return maps.back()->getCenter(index);
}

void InfluenceMapQuad::update(Physical* thing, float value) {
	for (auto* map : maps) {
		map->update(thing, value);
	}
}

void InfluenceMapQuad::reset() {
	for (auto* map : maps) {
		map->reset();
	}
}

void InfluenceMapQuad::finishCalc() {
	for (auto* map : maps) {
		map->finishCalc();
	}
}

void InfluenceMapQuad::print(Urho3D::String name) {
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

int InfluenceMapQuad::getMaxElement() {
	assert(false);
	return 0;
}

int InfluenceMapQuad::getMaxElement(const std::array<int, 4>& indexes) {
	assert(false);
	return 0;
}

unsigned short InfluenceMapQuad::getResolution() {
	assert(false);
	return 0;
}

Urho3D::Vector2 InfluenceMapQuad::getCenter(int index) {
	assert(false);
	return {};
}
