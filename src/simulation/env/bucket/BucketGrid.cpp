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

	levelsCache = new std::vector<std::pair<short, short>*>*[RES_SEP_DIST];
	for (int i = 0; i < RES_SEP_DIST; ++i) {
		levelsCache[i] = getEnvIndexs((((double)MAX_SEP_DIST) / RES_SEP_DIST) * i);
	}

	iterators = new BucketIterator*[MAX_THREADS];
	for (int i = 0; i < MAX_THREADS; ++i) {
		iterators[i] = new BucketIterator();
	}

	empty = new std::vector<Physical*>();
}

BucketGrid::~BucketGrid() {
	delete[] iterators;
}

void BucketGrid::updateGrid(Physical* entity, short team) {
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

std::vector<std::pair<short, short>*>* BucketGrid::getEnvIndexsFromCache(double dist) {
	int index = dist / diff;
	return levelsCache[index];
}

int BucketGrid::getIntegerPos(double value) {
	if (value < 0) {
		return (int)(value / size * resolution) - 1;
	}
	return (int)(value / size * resolution);
}

BucketIterator* BucketGrid::getArrayNeight(Unit* entity, double radius, short thread) {
	Vector3* pos = entity->getPosition();
	int dX = getIntegerPos(pos->x_);
	int dZ = getIntegerPos(pos->z_);

	BucketIterator* bucketIterator = iterators[thread];
	bucketIterator->init(getEnvIndexsFromCache(radius), dX, dZ, this);
	return bucketIterator;
}

void BucketGrid::removeAt(short x, short z, Physical* entity) {
	int posX = x + halfResolution;
	int posZ = z + halfResolution;

	if (isInSide(posX, posZ)) {
		buckets[posX][posZ].remove(entity);
	}
}

void BucketGrid::addAt(short x, short z, Physical* entity) {
	int posX = x + halfResolution;
	int posZ = z + halfResolution;

	if (isInSide(posX, posZ)) {
		buckets[posX][posZ].add(entity);
	}
}

std::vector<Physical*>* BucketGrid::getContentAt(short x, short z) {
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
			vector<Physical *>* content = getContentAt(i, j);
			entities->insert(entities->end(), content->begin(), content->end());
		}
	}
	return entities;
}

bool BucketGrid::fieldInCircle(int i, int j, double radius) {
	int x = i * fieldSize;
	int y = j * fieldSize;
	if (x * x + y * y < radius * radius) {
		return true;
	}
	return false;
}

std::vector<std::pair<short, short>*>* BucketGrid::getEnvIndexs(double radius) {
	std::vector<std::pair<short, short>*>* indexes = new std::vector<std::pair<short, short>*>();
	for (int i = 0; i < RES_SEP_DIST; i++) {
		for (int j = 0; j < RES_SEP_DIST; j++) {
			if (fieldInCircle(i, j, radius)) {
				int x = i + 1;
				int y = j + 1;
				indexes->push_back(new std::pair<short, short>(x, y));
				indexes->push_back(new std::pair<short, short>(x, -y));
				indexes->push_back(new std::pair<short, short>(-x, y));
				indexes->push_back(new std::pair<short, short>(-x, -y));
			}
		}
		if (fieldInCircle(i, 0, radius)) {
			int x = i + 1;
			indexes->push_back(new std::pair<short, short>(x, 0));
			indexes->push_back(new std::pair<short, short>(0, x));
			indexes->push_back(new std::pair<short, short>(-x, 0));
			indexes->push_back(new std::pair<short, short>(0, -x));
		}
	}
	indexes->push_back(new std::pair<short, short>(0, 0));
	return indexes;
}