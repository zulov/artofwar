#include "StaticGrid.h"

#include <cassert>
#include <iostream>

#include "Bucket.h"
#include "levels/LevelCache.h"
#include "objects/Physical.h"

StaticGrid::StaticGrid(short resolution, float size, std::vector<float> queryRadius): Grid(resolution, size, true,
	queryRadius.back()), queryRadius(queryRadius) {
	assert(queryRadius.size()>1);
	bucketsPerRadius.reserve(queryRadius.size());

	for (int i = 0; i < queryRadius.size(); ++i) {
		bucketsPerRadius.push_back(new Bucket[sqResolution]);
	}
}

StaticGrid::~StaticGrid() {
	for (const auto buckets : bucketsPerRadius) {
		delete[] buckets;
	}
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
			if (calculator->isValidIndex(shiftCords.x_ + centerCords.x_,shiftCords.y_ + centerCords.y_)) {
				itr[centerIndex + *ptrIdx].remove(physical);
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
			assert(indexes->size() == cords->size());
			const auto itr = bucketsPerRadius[i];

			auto ptrIdx = indexes->begin();
			auto x = itr + centerIndex;
			for (const auto& shiftCords : *cords) {
				if (calculator->isValidIndex(shiftCords.x_ + centerCords.x_,shiftCords.y_ + centerCords.y_)) {
					(*(x + *ptrIdx)).add(staticObj);
				}
				++ptrIdx;
			}
		}
	}
}

void StaticGrid::initAdd() const {
	unsigned short* sizes = new unsigned short[sqResolution];
	for (int i = 0; i < queryRadius.size(); ++i) {
		std::fill_n(sizes, sqResolution, 0);

		auto const [indexes, cords] = levelCache->getBoth(queryRadius.at(i));
		assert(indexes->size() == cords->size());
		for (int k = 0; k < sqResolution; ++k) {
			auto& bucket = buckets[k];

			if (bucket.getSize() > 0) {
				const auto centerCords = calculator->getIndexes(k);

				auto ptrIdx = indexes->begin();
				const auto ptrSize = sizes + k;
				for (const auto& shiftCords : *cords) {
					if (calculator->isValidIndex(shiftCords.x_ + centerCords.x_,shiftCords.y_ + centerCords.y_)) {
						*(ptrSize + *ptrIdx) += bucket.getContent().size();
					}
					++ptrIdx;
				}
			}
		}
		auto itr = bucketsPerRadius[i];
		for (int k = 0; k < sqResolution; ++k) {
			const auto val = sizes[k];
			if (val > 0) {
				itr->reserve(val);
			}
			++itr;
		}
	}

	delete[]sizes;

	for (int k = 0; k < sqResolution; ++k) {
		auto& bucket = buckets[k];

		if (bucket.getSize() > 0) {
			const auto centerCords = calculator->getIndexes(k);
			for (int i = 0; i < queryRadius.size(); ++i) {
				auto const [indexes, cords] = levelCache->getBoth(queryRadius.at(i));
				assert(indexes->size() == cords->size());
				const auto itr = bucketsPerRadius[i];

				auto ptrIdx = indexes->begin();
				auto x = itr + k;
				for (const auto& shiftCords : *cords) {
					if (calculator->isValidIndex(shiftCords.x_ + centerCords.x_,shiftCords.y_ + centerCords.y_)) {
						(*(x + *ptrIdx)).add(bucket.getContent());
					}
					++ptrIdx;
				}
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
