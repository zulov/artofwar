#include "InfluenceMapQuad.h"

#include <array>
#include "utils/DeleteUtils.h"

InfluenceMapQuad::InfluenceMapQuad(std::initializer_list<InfluenceMapI*> maps) {
	for (auto* map : maps) {
		this->maps.push_back(map);
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
		maps.at(i)->print(name + "_" + Urho3D::String(i));
	}
}

std::array<int, 4> InfluenceMapQuad::getIndexes(unsigned short resolution, int index) {
	std::array<int, 4> result;
	auto mod = index%resolution;
	auto div = index/resolution;
	mod*2+div*resolution*4
	
}

int InfluenceMapQuad::getMaxElement() {
	assert(false);
	return 0;
}

int InfluenceMapQuad::getMaxElement(std::array<int, 4> indexes) {
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
