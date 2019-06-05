#include "Grid.h"
#include "Bucket.h"
#include "BucketIterator.h"
#include "objects/Physical.h"
#include "objects/unit/Unit.h"
#include "simulation/env/ContentInfo.h"
#include <Urho3D/Graphics/Model.h>


Grid::Grid(short _resolution, float _size): resolution(_resolution),
                                            sqResolution(_resolution * _resolution),
                                            size(_size), fieldSize(_size / _resolution),
                                            halfResolution(_resolution / 2),
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

void Grid::update(Unit* unit, const char team) const {
	const int index = indexFromPosition(unit->getPosition());

	if (!unit->isAlive()) {
		removeAt(unit->getBucketIndex(team), unit);
	} else if (unit->bucketHasChanged(index, team)) {
		removeAt(unit->getBucketIndex(team), unit);
		addAt(index, unit);
		unit->setBucket(index, team);
	}
}

void Grid::update(Physical* entity) const {
	const int index = indexFromPosition(entity->getPosition());

	if (!entity->isAlive()) {
		removeAt(entity->getMainBucketIndex(), entity);
	} else if (entity->bucketHasChanged(index)) {
		removeAt(entity->getMainBucketIndex(), entity);
		addAt(index, entity);
		entity->setBucket(index);
	} else {
		entity->indexHasChangedReset();
	}
}

std::vector<short>* Grid::getEnvIndexsFromCache(float dist) {
	const int index = dist * invDiff;
	if (index < RES_SEP_DIST) {
		return levelsCache[index];
	}
	return levelsCache[RES_SEP_DIST - 1];
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

BucketIterator& Grid::getArrayNeight(Urho3D::Vector3& position, float radius, short thread) {
	return *iterators[thread].init(getEnvIndexsFromCache(radius), indexFromPosition(position), this);
}

void Grid::removeAt(int index, Physical* entity) const {
	if (inRange(index)) {
		buckets[index].remove(entity);
	}
}

void Grid::addAt(int index, Physical* entity) const {
	buckets[index].add(entity);
}

std::vector<Physical*>& Grid::getContentAt(int index) {
	if (inRange(index)) {
		return buckets[index].getContent();
	}
	return empty;
}

short Grid::diff(const short a, const short b) {
	auto diff = Urho3D::Sign(b - a);
	if (diff == 0) { return 1; }
	return diff;
}

std::vector<Physical*>* Grid::getArrayNeight(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair) {
	tempSelected->clear();

	const auto posBeginX = getIndex(pair.first->x_);
	const auto posBeginZ = getIndex(pair.first->z_);
	const auto posEndX = getIndex(pair.second->x_);
	const auto posEndZ = getIndex(pair.second->z_);

	auto dX = diff(posBeginX, posEndX);
	auto dZ = diff(posBeginZ, posEndZ);

	for (short i = posBeginX; i != posEndX + dX; i += dX) {
		for (short j = posBeginZ; j != posEndZ + dZ; j += dZ) {
			auto& content = getContentAt(getIndex(i, j));
			tempSelected->insert(tempSelected->end(), content.begin(), content.end());
		}
	}

	return tempSelected;
}

std::vector<Physical*>* Grid::getArrayNeightSimilarAs(Physical* clicked, double radius) {
	//TOODO clean prawie to samo co wy¿ej
	tempSelected->clear();

	const auto posBeginX = getIndex(clicked->getPosition().x_ - radius);
	const auto posBeginZ = getIndex(clicked->getPosition().z_ - radius);
	const auto posEndX = getIndex(clicked->getPosition().x_ + radius);
	const auto posEndZ = getIndex(clicked->getPosition().z_ + radius);


	auto dX = diff(posBeginX, posEndX);
	auto dZ = diff(posBeginZ, posEndZ);

	for (short i = posBeginX; i != posEndX + dX; i += dX) {
		for (short j = posBeginZ; j != posEndZ + dZ; j += dZ) {
			auto& content = getContentAt(getIndex(i, j));
			for (auto thing : content) {
				if (thing->getDbID() == clicked->getDbID()) {
					tempSelected->push_back(thing);
				}
			}
		}
	}

	return tempSelected;
}

int Grid::indexFromPosition(Urho3D::Vector3& pos) const {
	return getIndex(getIndex(pos.x_), getIndex(pos.z_));
}

int Grid::indexFromPosition(Urho3D::Vector2& pos) const {
	return getIndex(getIndex(pos.x_), getIndex(pos.y_));
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
