#include "Grid.h"
#include "Bucket.h"
#include "BucketIterator.h"
#include "objects/Physical.h"
#include "objects/unit/Unit.h"
#include "simulation/env/ContentInfo.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <algorithm>
#include <iostream>
#include <ostream>


Grid::Grid(short _resolution, float _size, bool _debugEnabled): resolution(_resolution),
	sqResolution(_resolution * _resolution),
	size(_size), fieldSize(_size / _resolution), debugEnabled(_debugEnabled), halfResolution(_resolution / 2),
	invFieldSize(_resolution / _size) {

	for (int i = 0; i < RES_SEP_DIST; ++i) {
		levelsCache[i] = getEnvIndexs((double)MAX_SEP_DIST / RES_SEP_DIST * i);
	}

	buckets = new Bucket[sqResolution];
	tempSelected = new std::vector<Physical*>();
}

Grid::~Grid() {
	for (auto& cache : levelsCache) {
		delete cache;
		cache = nullptr;
	}

	delete[] buckets;
	delete tempSelected;
}

void Grid::updateGrid(Unit* entity, const char team) const {
	const int index = indexFromPosition(entity->getPosition());

	if (!entity->isAlive()) {
		removeAt(entity->getBucketIndex(team), entity);
	} else if (entity->bucketHasChanged(index, team)) {
		removeAt(entity->getBucketIndex(team), entity);
		addAt(index, entity);
		entity->setBucket(index, team);
	}
}


std::vector<short>* Grid::getEnvIndexsFromCache(float dist) {
	const int index = dist * invDiff;
	return levelsCache[index];
}

short Grid::getIndex(float value) const {
	if (value < 0) {
		short index = (short)(value * invFieldSize) + halfResolution - 1;
		if (index >= 0) {
			return index;
		}
		return 0;
	}
	short index = (short)(value * invFieldSize) + halfResolution;
	if (index < resolution) {
		return index;
	}
	return resolution - 1; //TODO czy aby napewno?
}

BucketIterator& Grid::getArrayNeight(Unit* entity, float radius, short thread) {
	const int index = indexFromPosition(entity->getPosition());

	BucketIterator& bucketIterator = iterators[thread];
	bucketIterator.init(getEnvIndexsFromCache(radius), index, this);
	return bucketIterator;
}

void Grid::removeAt(int index, Unit* entity) const {
	if (inRange(index)) {
		buckets[index].remove(entity);
	}
}

void Grid::addAt(int index, Unit* entity) const {
	buckets[index].add(entity);
}

std::vector<Unit*>& Grid::getContentAt(int index) {
	if (inRange(index)) {
		return buckets[index].getContent();
	}
	return empty;
}

std::vector<Physical*>* Grid::getArrayNeight(std::pair<Vector3*, Vector3*>& pair) {
	Vector3* begin = pair.first;
	Vector3* end = pair.second;
	tempSelected->clear();

	const auto posBeginX = getIndex(begin->x_);
	const auto posBeginZ = getIndex(begin->z_);
	const auto posEndX = getIndex(end->x_);
	const auto posEndZ = getIndex(end->z_);

	for (short i = Min(posBeginX, posEndX); i <= Max(posBeginX, posEndX); ++i) {
		for (short j = Min(posBeginZ, posEndZ); j <= Max(posBeginZ, posEndZ); ++j) {
			const int index = getIndex(i, j);
			std::vector<Unit *>& content = getContentAt(index); //TODO czy tu ampersentma byc?
			tempSelected->insert(tempSelected->end(), content.begin(), content.end());
		}
	}
	return tempSelected;
}

int Grid::indexFromPosition(Vector3* position) const {
	const auto posX = getIndex(position->x_);
	const auto posZ = getIndex(position->z_);
	return getIndex(posX, posZ);
}

int Grid::indexFromPosition(Vector2& position) const {
	const auto posX = getIndex(position.x_);
	const auto posZ = getIndex(position.y_);
	return getIndex(posX, posZ);
}

bool Grid::fieldInCircle(short i, short j, float radius) const {
	const short x = i * fieldSize;
	const short y = j * fieldSize;
	return x * x + y * y < radius * radius;
}

std::vector<short>* Grid::getEnvIndexs(float radius) const {
	auto indexes = new std::vector<short>();
	for (short i = 0; i < RES_SEP_DIST; ++i) {
		for (short j = 0; j < RES_SEP_DIST; ++j) {
			if (fieldInCircle(i, j, radius)) {
				const short x = i + 1;
				const short y = j + 1;
				indexes->push_back(getIndex(x, y));
				indexes->push_back(getIndex(x, -y));
				indexes->push_back(getIndex(-x, y));
				indexes->push_back(getIndex(-x, -y));
			} else {
				break;
			}
		}
		if (fieldInCircle(i, 0, radius)) {
			const short x = i + 1;
			indexes->push_back(getIndex(x, 0));
			indexes->push_back(getIndex(0, x));
			indexes->push_back(getIndex(-x, 0));
			indexes->push_back(getIndex(0, -x));
		} else {
			break;
		}
	}
	indexes->push_back(getIndex(0, 0));
	std::sort(indexes->begin(), indexes->end());
	return indexes;
}
