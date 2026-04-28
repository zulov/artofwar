#include "StaticGrid.h"

#include <cassert>
#include <iostream>
#include <ranges>

#include "Bucket.h"
#include "ArrayProviderUtils.h"
#include "levels/LevelCache.h"
#include "objects/Physical.h"

StaticGrid::StaticGrid(short resolution, float size, std::vector<float> queryRadius): Grid(resolution, size,
			 queryRadius.back()), queryRadius(queryRadius) {
	assert(queryRadius.size()>1);
	bucketsPerRadius.reserve(queryRadius.size());

	for (int i = 0; i < queryRadius.size(); ++i) {
		bucketsPerRadius.push_back(ArrayProvider<Bucket>::get(sqResolution));
		inited.push_back(PrimitiveArrayProvider<bool>::get(sqResolution, false));
	}
}

StaticGrid::~StaticGrid() {
	for (auto* b : bucketsPerRadius) {
		ArrayProvider<Bucket>::release(b, sqResolution);
	}
	bucketsPerRadius.clear();
	for (auto* arr : inited) {
		PrimitiveArrayProvider<bool>::release(arr, sqResolution);
	}
	inited.clear();
}

void StaticGrid::remove(Physical* physical) const {
	Grid::remove(physical);
	const int centerIndex = calculator->indexFromPosition(physical->getPosition());

	for (int i = 0; i < queryRadius.size(); ++i) {
		const auto centerBucket = bucketsPerRadius[i] + centerIndex;
		const auto initedVec = inited[i] + centerIndex;
		const auto levels = levelCache->get(queryRadius.at(i), centerIndex);

		for (const auto idx : *levels) {
			if (*(initedVec + idx) == true) {
				(centerBucket + idx)->remove(physical);
			}
		}
	}
}

void StaticGrid::updateStatic(Static* staticObj, bool bulkAdd) const {
	Grid::updateNew(staticObj);
	if (!bulkAdd) {
		const int centerIndex = calculator->indexFromPosition(staticObj->getPosition());
		const auto centerCords = calculator->getCords(centerIndex);
		for (int i = 0; i < queryRadius.size(); ++i) {
			const auto centerBucket = bucketsPerRadius[i] + centerIndex;
			const auto initedVec = inited[i] + centerIndex;

			const auto levels = levelCache->get(queryRadius.at(i), centerCords);

			for (const auto idx : *levels) {
				if (*(initedVec + idx) == true) {
					(centerBucket + idx)->add(staticObj);
				}
			}
		}
	}
}

void StaticGrid::ensureInited(int index, int centerIndex) {
	if (inited[index][centerIndex]) { return; }
	inited[index][centerIndex] = true;

	auto& bucketRad = bucketsPerRadius[index][centerIndex];

	const auto levels = levelCache->get(queryRadius.at(index), centerIndex);

	for (const auto idx : *levels) {
		bucketRad.add(buckets[centerIndex + idx].getContent());
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
