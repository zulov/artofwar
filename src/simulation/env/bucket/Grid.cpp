#include "Grid.h"
#include "BucketIterator.h"
#include "simulation/env/ContentInfo.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <algorithm>
#include <iostream>
#include <ostream>


Grid::Grid(short _resolution, double _size, bool _debugEnabled) {
	resolution = _resolution;
	halfResolution = resolution / 2;
	size = _size;
	fieldSize = size / resolution;
	invFieldSize = resolution / size;
	debugEnabled = _debugEnabled;

	for (int i = 0; i < RES_SEP_DIST; ++i) {
		levelsCache[i] = getEnvIndexs((double)MAX_SEP_DIST / RES_SEP_DIST * i);
	}

	for (auto & iterator : iterators) {
		iterator = new BucketIterator();
	}
	buckets = new Bucket[resolution * resolution];
	tempSelected = new std::vector<Physical*>();
}

Grid::~Grid() {
	for (auto &iterator : iterators) {
		delete iterator;
		iterator = nullptr;
	}
	for (auto &cache : levelsCache)  {
		delete cache;
		cache = nullptr;
	}

	delete[] buckets;
	delete tempSelected;
}

void Grid::updateGrid(Unit* entity, const char team) {
	Vector3* pos = entity->getPosition();
	const short posX = getIndex(pos->x_);
	const short posZ = getIndex(pos->z_);
	const int index = getIndex(posX, posZ);
	if (!entity->isAlive()) {
		removeAt(entity->getBucketIndex(team), entity);
	} else if (entity->bucketHasChanged(index, team)) {
		removeAt(entity->getBucketIndex(team), entity);
		addAt(index, entity);
		entity->setBucket(index, team);
	}
}


std::vector<short>* Grid::getEnvIndexsFromCache(double dist) {
	const int index = dist / diff;
	return levelsCache[index];
}

short Grid::getIndex(double value) const {
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
	return resolution - 1;//TODO czy aby napewno?
}

BucketIterator* Grid::getArrayNeight(Unit* entity, double radius, short thread) {
	Vector3* pos = entity->getPosition();
	int index = getIndex(getIndex(pos->x_), getIndex(pos->z_));

	BucketIterator* bucketIterator = iterators[thread];
	bucketIterator->init(getEnvIndexsFromCache(radius), index, this);
	return bucketIterator;
}

void Grid::removeAt(int index, Unit* entity) {
	if (inRange(index)) {
		buckets[index].remove(entity);
	}
}

void Grid::addAt(int index, Unit* entity) {
	buckets[index].add(entity);
}

bool Grid::inRange(int index) {
	return index >= 0 && index < resolution * resolution;
}

std::vector<Unit*>& Grid::getContentAt(int index) {
	if (inRange(index)) {
		return buckets[index].getContent();
	}
	return empty;
}

int& Grid::getSizeAt(int index) {
	return buckets[index].getSize();
}

std::vector<Physical*>* Grid::getArrayNeight(std::pair<Vector3*, Vector3*>& pair) {
	Vector3* begin = pair.first;
	Vector3* end = pair.second;
	tempSelected->clear();

	const short posBeginX = getIndex(begin->x_);
	const short posBeginZ = getIndex(begin->z_);
	const short posEndX = getIndex(end->x_);
	const short posEndZ = getIndex(end->z_);

	for (short i = Min(posBeginX, posEndX); i <= Max(posBeginX, posEndX); ++i) {
		for (short j = Min(posBeginZ, posEndZ); j <= Max(posBeginZ, posEndZ); ++j) {
			const int index = i * resolution + j;
			std::vector<Unit *>& content = getContentAt(index);//TODO czy tu ampersentma byc?
			tempSelected->insert(tempSelected->end(), content.begin(), content.end());
		}
	}
	return tempSelected;
}

int Grid::getIndex(short posX, short posZ) {
	return posX * resolution + posZ;
}

bool Grid::fieldInCircle(short i, short j, double radius) {
	short x = i * fieldSize;
	short y = j * fieldSize;
	return x * x + y * y < radius * radius;
}

std::vector<short>* Grid::getEnvIndexs(double radius) {
	auto indexes = new std::vector<short>();
	for (short i = 0; i < RES_SEP_DIST; ++i) {
		for (short j = 0; j < RES_SEP_DIST; ++j) {
			if (fieldInCircle(i, j, radius)) {
				short x = i + 1;
				short y = j + 1;
				indexes->push_back(getIndex(x, y));
				indexes->push_back(getIndex(x, -y));
				indexes->push_back(getIndex(-x, y));
				indexes->push_back(getIndex(-x, -y));
			} else {
				break;
			}
		}
		if (fieldInCircle(i, 0, radius)) {
			short x = i + 1;
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
