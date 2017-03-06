#include "BucketGrid.h"


BucketGrid::BucketGrid(double _resolution, double _size) {
	resolution = _resolution;
	size = _size;
	bucketList.reserve(resolution);
	for (int i = 0; i < resolution; i++) {
		std::vector<Bucket *> list;
		list.reserve(resolution);
		bucketList.push_back(list);
	}
	for (int i = 0; i < resolution; i++) {
		for (int j = 0; j < resolution; j++) {
			bucketList[i].push_back(new Bucket());
		}
	}
	edgeBuckets.reserve(64);
	for (int i = 0; i < 64; i++) {
		edgeBuckets.push_back(new Bucket());
	}
	maxContentCount = 0;
	maxSum = 0;
	cache = new std::unordered_map<long, std::vector<Unit *>*>();
}

void BucketGrid::writeToGrid(std::vector<Unit*>* entitys) {
	for (int i = 0; i < entitys->size(); i++) {
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
	if (value < 0) { return (int)(value / size * (resolution)) - 1; } else { return (int)(value / size * (resolution)); }
}

int BucketGrid::cacheKey(int dX, int dZ) {
	return ((dX+1000) << 10) + dZ;
}

std::vector<Unit*>* BucketGrid::getArrayNeight(Unit* entity) {
	int level = entity->getLevelOfBucket();
	int dX = entity->getBucketX();
	int dZ = entity->getBucketZ();
	long key = cacheKey(dX, dZ);
	auto iter = cache->find(key);
	if (iter != cache->end()) {
		return iter->second;
	} else {
		std::vector<Unit*>* crowd = new std::vector<Unit *>();
		crowd->reserve(20);

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
		cache->insert(std::pair<long, std::vector <Unit*>* >(key, crowd));
		return crowd;
	}
}

Bucket* BucketGrid::getBucketAt(int _x, int _z) {
	int posX = _x + resolution / 2;
	int posZ = _z + resolution / 2;

	int index = getIndex(posX, posZ);
	if (index == 0) {
		return bucketList.at(posX).at(posZ);
	} else {
		return edgeBuckets[index];
	}
}

int BucketGrid::getIndex(int _posX, int _posZ) {
	int index = 0;

	if (_posX < 0) {
		index += 1;
	} else if (_posX >= resolution) {
		index += 2;
	}

	if (_posZ < 0) {
		index += 4;
	} else if (_posZ >= resolution) {
		index += 8;
	}

	return index;
}

int BucketGrid::calcXCordFromEdge(int index) {
	if ((-index) % 2 == 1) {
		return -1;
	} else if (((-index) >> 1) % 2 == 1) {
		return 1;
	} else return 0;
}

int BucketGrid::calcYCordFromEdge(int index) {
	if (((-index) >> 2) % 2 == 1) {
		return -1;
	} else if (((-index) >> 3) % 2 == 1) {
		return 1;
	} else return 0;
}

void BucketGrid::updateSums() {
	for (int i = 0; i < resolution; i++) {
		for (int j = 0; j < resolution; j++) {
			bucketList.at(i).at(j)->addToSum();
		}
	}
}

std::vector<std::vector<Bucket*>> BucketGrid::getBucketList() {
	return bucketList;
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
	cache->clear();
}
