#include "StaticGrid.h"

#include <cassert>
#include <iostream>

#include "Bucket.h"
#include "levels/LevelCache.h"
#include "objects/Physical.h"
#include "utils/DeleteUtils.h"

StaticGrid::StaticGrid(short resolution, float size, std::vector<float> queryRadius): Grid(resolution, size, true,
	queryRadius.back()), queryRadius(queryRadius) {
	assert(queryRadius.size()>1);
	bucketsPerRadius.reserve(queryRadius.size());

	for (int i = 0; i < queryRadius.size(); ++i) {
		bucketsPerRadius.push_back(new Bucket[sqResolution]);
		bool* arr = new bool[sqResolution];
		std::fill_n(arr, sqResolution, false);
		inited.push_back(arr);
	}
}

StaticGrid::~StaticGrid() {
	clear_vector_array(bucketsPerRadius);
	clear_vector_array(inited);
}

int StaticGrid::update(Physical* entity, int currentIndex) const {
	assert(false);
	return -1;
}

void StaticGrid::remove(Physical* physical) const {
	Grid::remove(physical);
	const int centerIndex = calculator->indexFromPosition(physical->getPosition());
	const auto centerCords = calculator->getIndexes(centerIndex);
	for (int i = 0; i < queryRadius.size(); ++i) {
		auto const [indexes, cords] = levelCache->getBoth(queryRadius.at(i));
		assert(indexes->size() == cords->size());
		const auto itr = bucketsPerRadius[i];

		auto ptrIdx = indexes->begin();
		for (const auto& shiftCords : *cords) {
			if (calculator->isValidIndex(shiftCords.x_ + centerCords.x_, shiftCords.y_ + centerCords.y_)) {
				if (inited[i][centerIndex + *ptrIdx]) {
					itr[centerIndex + *ptrIdx].remove(physical); //TODDO notafe or if inited
				}
			}
			++ptrIdx;
		}
	}
}

void StaticGrid::updateStatic(Static* staticObj, bool bulkAdd) const {
	staticObj->setBucket(Grid::updateNew(staticObj));

	if (!bulkAdd) {
		const int centerIndex = calculator->indexFromPosition(staticObj->getPosition());
		const auto centerCords = calculator->getIndexes(centerIndex);
		for (int i = 0; i < queryRadius.size(); ++i) {
			auto const [indexes, cords] = levelCache->getBoth(queryRadius.at(i));
			const auto itr = bucketsPerRadius[i];

			auto ptrIdx = indexes->begin();
			auto x = itr + centerIndex;
			for (const auto& shiftCords : *cords) {
				if (calculator->isValidIndex(shiftCords.x_ + centerCords.x_, shiftCords.y_ + centerCords.y_)) {
					if (inited[i][centerIndex + *ptrIdx]) {
						(*(x + *ptrIdx)).add(staticObj);
					}
				}
				++ptrIdx;
			}
		}
	}
}

void StaticGrid::ensureInited(int index, int centerIndex) {
	if (inited[index][centerIndex]) { return; }
	inited[index][centerIndex] = true;

	auto const [indexes, cords] = levelCache->getBoth(queryRadius.at(index));
	const auto centerCords = calculator->getIndexes(centerIndex);

	auto ptrIdx = indexes->begin();
	//TODO pref? reserver size
	auto& bucketRad = bucketsPerRadius[index][centerIndex];

	for (const auto& shiftCords : *cords) {
		if (calculator->isValidIndex(shiftCords.x_ + centerCords.x_, shiftCords.y_ + centerCords.y_)) {
			bucketRad.add(buckets[centerIndex + *ptrIdx].getContent());
		}
		++ptrIdx;
	}
}

const std::vector<Physical*>& StaticGrid::get(const Urho3D::Vector3& center, float radius) {
	const int index = getIndexForRadius(radius);
	const int centerIndex = calculator->indexFromPosition(center);

	ensureInited(index, centerIndex);

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
