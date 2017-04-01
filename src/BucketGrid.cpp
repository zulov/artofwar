#include "BucketGrid.h"


BucketGrid::BucketGrid(int _resolution, double _size) {
	resolution = _resolution;
	size = _size;
	fieldSize = size / resolution;

	bucketList.reserve(resolution);
	for (int i = 0; i < resolution; i++) {
		std::vector<Bucket *> list;
		list.reserve(resolution);
		bucketList.push_back(list);
	}
	for (int i = 0; i < resolution; ++i) {
		for (int j = 0; j < resolution; ++j) {
			bucketList[i].push_back(new Bucket());
		}
	}

	cache = new std::vector<Unit*>*[resolution * resolution];
	for (int i = 0; i < resolution * resolution; ++i) {
		cache[i] = nullptr;
	}
}

void BucketGrid::writeToGrid(std::vector<Unit*>* entitys) {
	for (int i = 0; i < entitys->size(); ++i) {
		Vector3 pos = entitys->at(i)->getPosition();
		int posX = getIntegerPos(pos.x_);
		int posZ = getIntegerPos(pos.z_);

		entitys->at(i)->setBucket(posX, posZ);
		getBucketAt(posX, posZ)->add(entitys->at(i));
	}
}

void BucketGrid::updateGrid(Unit* entity) {
	Vector3 pos = entity->getPosition();
	int posX = getIntegerPos(pos.x_);
	int posZ = getIntegerPos(pos.z_);
	if (!entity->isAlive()) {
		getBucketAt(entity->getBucketX(), entity->getBucketZ())->remove(entity);
	} else if (entity->bucketHasChanged(posX, posZ)) {
		getBucketAt(entity->getBucketX(), entity->getBucketZ())->remove(entity);
		getBucketAt(posX, posZ)->add(entity);
		entity->setBucket(posX, posZ);
	}
}

int BucketGrid::getIntegerPos(double value) {
	if (value < 0) {
		return (int)(value / size * (resolution)) - 1;
	} else {
		return (int)(value / size * (resolution));
	}
}

int BucketGrid::cacheHash(int dX, int dZ) {
	int x = dX + resolution / 2;
	int z = dZ + resolution / 2;

	return (int)resolution * x + z;
}

void BucketGrid::updateSizes(int size) {
	lastSize = size;
	if (maxSize < size) {
		maxSize = size;
	}
}

std::vector<Unit*>* BucketGrid::getArrayNeight(Unit* entity) {
	double unitRadius = entity->getUnitRadius();
	double cubeSize = size / resolution;//b
	int level = unitRadius / cubeSize + 0.99;
	int dX = entity->getBucketX();
	int dZ = entity->getBucketZ();
	long key = cacheHash(dX, dZ);

	if (cache[key] != nullptr) {
		return cache[key];
	} else {
		std::vector<Unit*>* crowd = new std::vector<Unit *>();
		crowd->reserve((lastSize + maxSize) / 2);
		std::vector<std::pair<int, int>*>* levels = getEnvIndexs(entity->getMaxSeparationDistance());
		int sqLevel = level * level;
		for (int i = 0; i < levels->size(); i++) {
			std::pair<int, int>* pair = levels->at(i);
			Bucket* bucket = getBucketAt(pair->first + dX, pair->second + dZ);
			std::vector<Unit *>* content = bucket->getContent();
			if (!content->empty()) {
				crowd->insert(crowd->end(), content->begin(), content->end());
			}
		}
		cache[key] = crowd;

		updateSizes(crowd->size());
		for (int i = 0; i< levels->size(); i++){
			delete (levels->at(i));
		}
		levels->clear();
		delete levels;
		return crowd;
	}
}

Bucket* BucketGrid::getBucketAt(int _x, int _z) {
	int posX = _x + resolution / 2;
	int posZ = _z + resolution / 2;

	if (isInSide(posX, posZ)) {
		return bucketList.at(posX).at(posZ);
	} else {
		return new Bucket();
	}
}

bool BucketGrid::isInSide(int _posX, int _posZ) const {
	if (_posX < 0 || _posX >= resolution || _posZ < 0 || _posZ >= resolution) {
		return false;
	} else {
		return true;
	}
}

void BucketGrid::clean() {
	/*for (int i = 0; i < resolution; i++) {
		for (int j = 0; j < resolution; j++) {
			if (bucketList[i] != nullptr &&bucketList[i]->default[j] != nullptr) {
				bucketList[i]->default[j]->removeCube();
			}

		}
	}*/
}

void BucketGrid::clearAfterStep() {
	for (int i = 0; i < resolution * resolution; ++i) {
		if (cache[i] != nullptr) {
			cache[i]->clear();
			delete cache[i];
			cache[i] = nullptr;
		}
	}
}

bool BucketGrid::fieldInCircle(int i, int j, double radius) {
	int x = i * fieldSize;
	int y = j * fieldSize;
	if (x * x + y * y < radius * radius) {
		return true;
	} else {
		return false;
	}
}

std::vector<std::pair<int, int>*>* BucketGrid::getEnvIndexs(double radius) {
	std::vector<std::pair<int, int>*>* indexes = new std::vector<std::pair<int, int>*>();

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (fieldInCircle(i, j, radius)) {
				int x = i + 1;
				int y = j + 1;
				indexes->push_back(new std::pair<int, int>(x, y));
				indexes->push_back(new std::pair<int, int>(x, -y));
				indexes->push_back(new std::pair<int, int>(-x, y));
				indexes->push_back(new std::pair<int, int>(-x, -y));
			}
		}
		if (fieldInCircle(i, 0, radius)) {
			int x = i + 1;
			indexes->push_back(new std::pair<int, int>(x, 0));
			indexes->push_back(new std::pair<int, int>(0, x));
			indexes->push_back(new std::pair<int, int>(-x, 0));
			indexes->push_back(new std::pair<int, int>(0, -x));
		}
	}
	indexes->push_back(new std::pair<int, int>(0, 0));
	return indexes;
}
