#include "BucketGrid.h"


BucketGrid::BucketGrid(int _resolution, double _size) {
	resolution = _resolution;
	size = _size;
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
	int level = entity->getLevelOfBucket();
	int dX = entity->getBucketX();
	int dZ = entity->getBucketZ();
	long key = cacheHash(dX, dZ);

	if (cache[key] != nullptr) {
		return cache[key];
	} else {
		std::vector<Unit*>* crowd = new std::vector<Unit *>();
		crowd->reserve((lastSize + maxSize) / 2);

		int sqLevel = level * level;
		for (int i = -level; i <= level; i++) {
			for (int j = -level; j <= level; j++) {
				if (sqLevel >= i * i + j * j) {
					Bucket* bucket = getBucketAt(i + dX, j + dZ);
					std::vector<Unit *>* content = bucket->getContent();
					if (!content->empty()) {
						crowd->insert(crowd->end(), content->begin(), content->end());
					}
				}
			}
		}
		cache[key] = crowd;

		updateSizes(crowd->size());
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
	}else {
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
