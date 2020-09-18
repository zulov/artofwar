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
		std::array<int, 4> indexes = getIndexes(i,index);
		index = maps[i]->getMaxElement(indexes);
	}
	maps.back()->getC
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
