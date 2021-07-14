#include "StaticGrid.h"

#include <cassert>
#include <iostream>

#include "Bucket.h"
#include "levels/LevelCache.h"
#include "objects/Physical.h"

StaticGrid::StaticGrid(short resolution, float size, std::vector<float> queryRadius): Grid(resolution, size,
	queryRadius.back()), queryRadius(queryRadius) {
	assert(queryRadius.size()>1);
	bucketsPerRadius.reserve(queryRadius.size());
	int defSize = 300;
	for (int i = 0; i < queryRadius.size(); ++i) {
		auto buckets = new Bucket[resolution * resolution];
		bucketsPerRadius.push_back(buckets);
		for (int i = 0; i < resolution * resolution; ++i) {
			buckets[i].reserve(defSize);
		}
		defSize *= 3;
	}
}

StaticGrid::~StaticGrid() {
	for (auto buckets : bucketsPerRadius) {
		// for (int i = 0; i < calculator->getResolution() * calculator->getResolution(); ++i) {
		// 	
		//
		// 	std::cout << buckets[i].getSize()<<";";
		// }
		// std::cout << std::endl;
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


inline bool StaticGrid::inside(int val) const { return val >= 0 && val < calculator->getResolution(); }

void StaticGrid::updateNew(Physical* physical) const {
	Grid::updateNew(physical);

	const int centerIndex = calculator->indexFromPosition(physical->getPosition());
	const auto centerCords = calculator->getIndexes(centerIndex);
	for (int i = 0; i < queryRadius.size(); ++i) {
		for (auto value : *levelCache->get(queryRadius.at(i))) {
			const auto shiftCords = calculator->getShiftCords(value);
			//TODO cache

			if (inside(shiftCords.x_ + centerCords.x_) && inside(shiftCords.y_ + centerCords.y_)) {
				bucketsPerRadius[i][centerIndex + value].add(physical);
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
