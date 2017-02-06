#include "BucketGrid.h"


BucketGrid::BucketGrid(double _resolution, double _size) {
	resolution = _resolution;
	size = _size;

	for (int i = 0; i < resolution; i++) {
		std::vector<Bucket *> list;
		bucketList.push_back(list);
	}
	for (int i = 0; i < resolution; i++) {
		for (int j = 0; j < resolution; j++) {
			bucketList[i].push_back(new Bucket());
		}
	}

	for (int i = 0; i < 64; i++) {
		edgeBuckets.push_back(new Bucket());
	}
	maxContentCount = 0;
	maxSum = 0;
}
void BucketGrid::writeToGrid(std::vector<Unit*> entitys) {
	for (int i = 0; i < entitys.size(); i++) {
		Vector3  pos = entitys[i]->getPosition();
		int posX = getIntegerPos(pos.x_);
		int posY = getIntegerPos(pos.y_);

		entitys[i]->setBucket(posX, posY);
		getBucketAt(posX, posY)->add(entitys[i]);
	}
}

void BucketGrid::updateGrid(Unit* entity) {
	Vector3 pos = entity->getPosition();
	int posX = getIntegerPos(pos.x_);
	int posY = getIntegerPos(pos.y_);
	if (!entity->isAlive()) {
		getBucketAt(entity->getBucketX(), entity->getBucketY())->remove(entity);
	} else if (!entity->checkBucketXY(posX, posY)) {
		getBucketAt(entity->getBucketX(), entity->getBucketY())->remove(entity);
		getBucketAt(posX, posY)->add(entity);
		entity->setBucket(posX, posY);
	}
}

int BucketGrid::getIntegerPos(double value) {
	if (value < 0) { return  (int)(value / size*(resolution)) - 1; } else { return  (int)(value / size*(resolution)); }
}

std::vector <Unit*> BucketGrid::getArrayNeight(Unit* entity) {
	std::vector<Unit*> crowd;
	int level = entity->getLevelOfBucket();
	int dX = entity->getBucketX();
	int dY = entity->getBucketY();

	int sqLevel = level*level;

	for (int i = -level; i <= level; i++) {
		for (int j = -level; j <= level; j++) {
			if (sqLevel >= i*i + j*j) {
				Bucket * bucket = getBucketAt(i + dX, j + dY);
				std::vector<Unit *> content = bucket->getContent();
				if (!content.empty()) {
					crowd.insert(crowd.end(), content.begin(), content.end());
				}
				
			}
		}
	}

	return crowd;
}

Bucket *BucketGrid::getBucketAt(int _x, int _y) {
	int posX = _x + resolution / 2;
	int posY = _y + resolution / 2;

	int index = getIndex(posX, posY);
	if (index == 0) {
		return bucketList.at(posX).at(posY);
	} else {
		return edgeBuckets[index];
	}
}

int BucketGrid::getIndex(int _posX, int _posY) {
	int index = 0;

	if (_posX < 0) {
		index += 1;
	} else if (_posX >= resolution) {
		index += 2;
	}

	if (_posY < 0) {
		index += 4;
	} else if (_posY >= resolution) {
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
