#include "StaticGrid.h"

#include <cassert>

#include "Bucket.h"
#include "levels/LevelCache.h"
#include "objects/Physical.h"

StaticGrid::StaticGrid(short resolution, float size, std::vector<float> queryRadius): Grid(resolution, size,
	queryRadius.back()), queryRadius(queryRadius) {
	assert(queryRadius.size()>1);
	bucketsPerRadius.reserve(queryRadius.size());
	for (int i = 0; i < queryRadius.size(); ++i) {
		bucketsPerRadius.push_back(new Bucket[resolution * resolution]);
	}
}

StaticGrid::~StaticGrid() {
	for (auto buckets : bucketsPerRadius) {
		delete[] buckets;
	}
}

void StaticGrid::update(Unit* unit, char team) const {
	assert(false);
}

void StaticGrid::update(Physical* entity) const {
	assert(false);
}

void StaticGrid::remove(Physical* physical) const {
	Grid::remove(physical);
	const int centerIndex = calculator->indexFromPosition(physical->getPosition());
	for (int i = 0; i < queryRadius.size(); ++i) {
		for (auto value : *levelCache->get(queryRadius.at(i))) {
			int newIndex = centerIndex + value; //TODO check boundry
			bucketsPerRadius[i][newIndex].remove(physical);
		}
	}
}

void StaticGrid::remove(Unit* unit, char team) const {
	assert(false);
}

void StaticGrid::updateNew(Unit* unit, char team) const {
	assert(false);
}


void StaticGrid::updateNew(Physical* physical) const {
	Grid::updateNew(physical);

	const int centerIndex = calculator->indexFromPosition(physical->getPosition());
	for (int i = 0; i < queryRadius.size(); ++i) {
		for (auto value : *levelCache->get(queryRadius.at(i))) {
			int newIndex = centerIndex + value; //TODO bug check boundry, na druga strone
			auto a = calculator->getIndexes(value);
			auto b = calculator->getIndexes(centerIndex);
			auto c= calculator->getIndexes(newIndex);
			auto x1 = a.x_ + b.x_;
			auto z1 = a.y_ + b.y_;
			if (x1 < 0 || x1 >= calculator->getResolution()) {
				int q = 5;
			}
			if (z1 < 0 || z1 >= calculator->getResolution()) {
				int q = 5;
			}
			if (calculator->isValidIndex(newIndex)) {
				bucketsPerRadius[i][newIndex].add(physical);
			}
		}
	}
}

const std::vector<Physical*>& StaticGrid::get(const Urho3D::Vector3& center, float radius) {
	const int index = getIndexForRadius(radius);
	const int centerIndex = calculator->indexFromPosition(center);
	return bucketsPerRadius[index][centerIndex].getContent();
}

int StaticGrid::getIndexForRadius(float radius) const {
	for (int i = 0; i < queryRadius.size(); ++i) {
		if (queryRadius[i] >= radius) {
			return i;
		}
	}
	return queryRadius.size() - 1;
}
