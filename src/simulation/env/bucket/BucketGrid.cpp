#include "BucketGrid.h"
#include "BucketIterator.h"
#include <algorithm>

BucketGrid::BucketGrid(short _resolution, double _size) {
	resolution = _resolution;
	halfResolution = resolution / 2;
	size = _size;
	fieldSize = size / resolution;
	buckets = new Bucket[resolution * resolution];

	levelsCache = new std::vector<int>*[RES_SEP_DIST];
	for (int i = 0; i < RES_SEP_DIST; ++i) {
		levelsCache[i] = getEnvIndexs((((double)MAX_SEP_DIST) / RES_SEP_DIST) * i);
	}

	iterators = new BucketIterator*[MAX_THREADS];
	for (int i = 0; i < MAX_THREADS; ++i) {
		iterators[i] = new BucketIterator();
	}

	empty = new std::vector<Unit*>();
}

BucketGrid::~BucketGrid() {
	delete[] iterators;
	delete empty;
	delete[] buckets;
}

void BucketGrid::updateGrid(Unit* entity, short team) {
	Vector3* pos = entity->getPosition();
	const short posX = getIndex(pos->x_);
	const short posZ = getIndex(pos->z_);
	const int index = posX * resolution + posZ;
	if (!entity->isAlive()) {
		removeAt(entity->getBucketIndex(team), entity);
	} else if (entity->bucketHasChanged(index, team)) {
		removeAt(entity->getBucketIndex(team), entity);
		addAt(index, entity);
		entity->setBucket(index, team);
	}
}

bool BucketGrid::validateAdd(Static* object) {
	Vector3* pos = object->getPosition();
	short posX = getIndex(pos->x_);
	short posZ = getIndex(pos->z_);
	const int index = posX * resolution + posZ;
	if (inRange(index) &&
		buckets[index].getType() == ObjectType::UNIT) {
		return true;
	}
	return false;
}

void BucketGrid::addStatic(Static* object) {
	//TODO duzo poprawek trzeba, rozmair œrodek validacja, sprawdzenie przedziaa³ów
	if (validateAdd(object)) {
		IntVector2 size = object->getGridSize();
		Vector3* pos = object->getPosition();
		short iX = getIndex(pos->x_);
		short iZ = getIndex(pos->z_);
		const int index = iX * resolution + iZ;
		object->setBucket(index, 0);
		for (short i = 0; i < size.x_; ++i) {
			for (short j = 0; j < size.y_; ++j) {
				const int index2 = (iX + i) * resolution + (iZ + j);
				buckets[index2].setStatic(object);
			}
		}
	}
}

void BucketGrid::removeStatic(Static* object) {
	int index = object->getBucketIndex(0);
	buckets[index].removeStatic();
}

std::vector<int>* BucketGrid::getEnvIndexsFromCache(double dist) {
	int index = dist / diff;
	return levelsCache[index];
}

short BucketGrid::getIndex(double value) {
	if (value < 0) {
		short index = (short)(value / size * resolution) + halfResolution - 1;
		if (index >= 0) {
			return index;
		}
	} else {
		short index = (short)(value / size * resolution) + halfResolution;
		if (index < resolution) {
			return index;
		}
	}
	return 0;
}

BucketIterator* BucketGrid::getArrayNeight(Unit* entity, double radius, short thread) {
	Vector3* pos = entity->getPosition();
	int index = getIndex(getIndex(pos->x_), getIndex(pos->z_));

	BucketIterator* bucketIterator = iterators[thread];
	bucketIterator->init(getEnvIndexsFromCache(radius), index, this);
	return bucketIterator;
}

void BucketGrid::removeAt(int index, Unit* entity) {
	if (inRange(index)) {
		buckets[index].remove(entity);
	}
}

void BucketGrid::addAt(int index, Unit* entity) {
	buckets[index].add(entity);
}

bool BucketGrid::inRange(int index) {
	return index >= 0 && index < resolution * resolution;
}

std::vector<Unit*>* BucketGrid::getContentAt(int index) {
	if (inRange(index)) {
		return buckets[index].getContent();
	}
	return empty;
}

std::vector<Physical*>* BucketGrid::getArrayNeight(std::pair<Vector3*, Vector3*>* pair) {
	Vector3* begin = pair->first;
	Vector3* end = pair->second;
	std::vector<Physical*>* entities = new std::vector<Physical*>();//TOODO reserva zrobic sensownego
	entities->reserve(DEFAULT_VECTOR_SIZE);
	short posBeginX = getIndex(begin->x_);
	short posBeginZ = getIndex(begin->z_);
	short posEndX = getIndex(end->x_);
	short posEndZ = getIndex(end->z_);

	for (short i = Min(posBeginX, posEndX); i <= Max(posBeginX, posEndX); ++i) {
		for (short j = Min(posBeginZ, posEndZ); j <= Max(posBeginZ, posEndZ); ++j) {
			const int index = i * resolution + j;
			vector<Unit *>* content = getContentAt(index);
			entities->insert(entities->end(), content->begin(), content->end());
		}
	}
	return entities;
}

int BucketGrid::getIndex(short posX, short posZ) {
	return posX * resolution + posZ;
}

Vector3* BucketGrid::validatePosition(Vector3* position) {
	short posX = getIndex(position->x_);
	short posZ = getIndex(position->z_);
	const int index = getIndex(posX, posZ);
	if (buckets[index].getType() != ObjectType::UNIT) {
		double bucketSize = (size / resolution);
		double cX = posX + (0.5) * bucketSize;
		double cZ = posZ + (0.5) * bucketSize;

		Vector3* direction = new Vector3(cX - position->x_, 0, cZ - position->z_);
		direction->Normalize();
		return direction;
	}
	return nullptr;
}

bool BucketGrid::fieldInCircle(short i, short j, double radius) {
	short x = i * fieldSize;
	short y = j * fieldSize;
	if (x * x + y * y < radius * radius) {
		return true;
	}
	return false;
}

std::vector<int>* BucketGrid::getEnvIndexs(double radius) {
	std::vector<int>* indexes = new std::vector<int>();
	for (short i = 0; i < RES_SEP_DIST; ++i) {
		for (short j = 0; j < RES_SEP_DIST; ++j) {
			if (fieldInCircle(i, j, radius)) {
				short x = i + 1;
				int y = j + 1;
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
