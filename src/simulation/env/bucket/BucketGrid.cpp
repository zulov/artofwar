#include "BucketGrid.h"
#include "BucketIterator.h"

BucketGrid::BucketGrid(short _resolution, double _size) {
	resolution = _resolution;
	halfResolution = resolution / 2;
	size = _size;
	fieldSize = size / resolution;
	buckets = new Bucket*[resolution];

	for (int i = 0; i < resolution; i++) {
		buckets[i] = new Bucket [resolution];
	}

	levelsCache = new std::vector<std::pair<char, char>>*[RES_SEP_DIST];
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
}

void BucketGrid::updateGrid(Unit* entity, short team) {
	Vector3* pos = entity->getPosition();
	int posX = getIntegerPos(pos->x_);
	int posZ = getIntegerPos(pos->z_);
	if (!entity->isAlive()) {
		removeAt(entity->getBucketX(team), entity->getBucketZ(team), entity);
	} else if (entity->bucketHasChanged(posX, posZ, team)) {
		removeAt(entity->getBucketX(team), entity->getBucketZ(team), entity);
		addAt(posX, posZ, entity);
		entity->setBucket(posX, posZ, team);
	}
}

bool BucketGrid::validateAdd(Static* object) {
	Vector3* pos = object->getPosition();
	int posX = getIntegerPos(pos->x_) + halfResolution;
	int posZ = getIntegerPos(pos->z_) + halfResolution;
	if (isInSide(posX, posZ) &&
		buckets[posX][posZ].getType() == ObjectType::UNIT) {
		return true;
	}
	return false;
}

void BucketGrid::addStatic(Static* object) {
	//TODO duzo poprawek trzeba, rozmair œrodek validacja
	if (validateAdd(object)) {
		IntVector2 size = object->getGridSize();
		Vector3* pos = object->getPosition();
		int posX = getIntegerPos(pos->x_) + halfResolution;
		int posZ = getIntegerPos(pos->z_) + halfResolution;
		object->setBucket(posX, posZ, 0);
		for (int i = 0; i < size.x_; ++i) {
			for (int j = 0; j < size.y_; ++j) {
				buckets[posX + i][posZ + j].setStatic(object);
			}
		}

	}
}

void BucketGrid::removeStatic(Static* object) {
	int posX = object->getBucketX(0);
	int posZ = object->getBucketZ(0);
	buckets[posX][posZ].removeStatic();
}

std::vector<std::pair<char, char>>* BucketGrid::getEnvIndexsFromCache(double dist) {
	int index = dist / diff;
	return levelsCache[index];
}

int BucketGrid::getIntegerPos(double value) {
	if (value < 0) {
		return (int)(value / size * resolution) - 1;
	}
	return (int)(value / size * resolution);
}

int BucketGrid::getIndex(double value) {
	return getIntegerPos(value) + halfResolution;
}

BucketIterator* BucketGrid::getArrayNeight(Unit* entity, double radius, short thread) {
	Vector3* pos = entity->getPosition();
	int dX = getIntegerPos(pos->x_);
	int dZ = getIntegerPos(pos->z_);

	BucketIterator* bucketIterator = iterators[thread];
	bucketIterator->init(getEnvIndexsFromCache(radius), dX, dZ, this);
	return bucketIterator;
}

void BucketGrid::removeAt(short x, short z, Unit* entity) {
	int posX = x + halfResolution;
	int posZ = z + halfResolution;

	if (isInSide(posX, posZ)) {
		buckets[posX][posZ].remove(entity);
	}
}

void BucketGrid::addAt(short x, short z, Unit* entity) {
	int posX = x + halfResolution;
	int posZ = z + halfResolution;

	if (isInSide(posX, posZ)) {
		buckets[posX][posZ].add(entity);
	}
}

std::vector<Unit*>* BucketGrid::getContentAt(short x, short z) {
	int posX = x + halfResolution;
	int posZ = z + halfResolution;

	if (isInSide(posX, posZ)) {
		return buckets[posX][posZ].getContent();
	}
	return empty;
}

bool BucketGrid::isInSide(int _posX, int _posZ) const {
	if (_posX < 0 || _posX >= resolution || _posZ < 0 || _posZ >= resolution) {
		return false;
	}
	return true;
}

std::vector<Physical*>* BucketGrid::getArrayNeight(std::pair<Vector3*, Vector3*>* pair) {
	Vector3* begin = pair->first;
	Vector3* end = pair->second;
	std::vector<Physical*>* entities = new std::vector<Physical*>();//TOODO reserva zrobic sensownego
	int posBeginX = getIntegerPos(begin->x_);
	int posBeginZ = getIntegerPos(begin->z_);
	int posEndX = getIntegerPos(end->x_);
	int posEndZ = getIntegerPos(end->z_);

	for (int i = Min(posBeginX, posEndX); i <= Max(posBeginX, posEndX); ++i) {
		for (int j = Min(posBeginZ, posEndZ); j <= Max(posBeginZ, posEndZ); ++j) {
			vector<Unit *>* content = getContentAt(i, j);
			entities->insert(entities->end(), content->begin(), content->end());
		}
	}
	return entities;
}

Vector3* BucketGrid::validatePosition(Vector3* position) {
	int posX = getIntegerPos(position->x_) + halfResolution;
	int posZ = getIntegerPos(position->z_) + halfResolution;

	if (isInSide(posX, posZ)) {

		if (buckets[posX][posZ].getType() != ObjectType::UNIT) {

			double bucketSize = (size / resolution);
			double cX = posX + (0.5) * bucketSize;
			double cZ = posZ + (0.5) * bucketSize;

			Vector3* direction = new Vector3(cX - position->x_, 0, cZ - position->z_);
			direction->Normalize();
			return direction;
		}
	}
	return nullptr;
}

bool BucketGrid::fieldInCircle(int i, int j, double radius) {
	int x = i * fieldSize;
	int y = j * fieldSize;
	if (x * x + y * y < radius * radius) {
		return true;
	}
	return false;
}

std::vector<std::pair<char, char>>* BucketGrid::getEnvIndexs(double radius) {
	std::vector<std::pair<char, char>>* indexes = new std::vector<std::pair<char, char>>();
	for (int i = 0; i < RES_SEP_DIST; i++) {
		for (int j = 0; j < RES_SEP_DIST; j++) {
			if (fieldInCircle(i, j, radius)) {
				int x = i + 1;
				int y = j + 1;
				indexes->push_back(std::pair<char, char>(x, y));
				indexes->push_back(std::pair<char, char>(x, -y));
				indexes->push_back(std::pair<char, char>(-x, y));
				indexes->push_back(std::pair<char, char>(-x, -y));
			}
		}
		if (fieldInCircle(i, 0, radius)) {
			int x = i + 1;
			indexes->push_back(std::pair<char, char>(x, 0));
			indexes->push_back(std::pair<char, char>(0, x));
			indexes->push_back(std::pair<char, char>(-x, 0));
			indexes->push_back(std::pair<char, char>(0, -x));
		}
	}
	indexes->push_back(std::pair<char, char>(0, 0));
	return indexes;
}
