#include "MainGrid.h"

MainGrid::MainGrid(short _resolution, double _size, bool _debugEnabled): Grid(_resolution, _size, _debugEnabled) {
	short posX = 0;
	short posZ = 0;

	int miniRes = resolution / 8;

	for (int i = 0; i < resolution * resolution; ++i) {
		buckets[i].upgrade();
		double cX = (posX + 0.5) * fieldSize - size / 2;
		double cZ = (posZ + 0.5) * fieldSize - size / 2;
		buckets[i].setCenter(cX, cZ);
		if (debugEnabled &&
			(cX > -miniRes && cX < miniRes) &&
			(cZ > -miniRes && cZ < miniRes)) {
			buckets[i].createBox(fieldSize);
		}
		++posZ;
		if (posZ >= resolution) {
			++posX;
			posZ = 0;
		}
	}
}

MainGrid::~MainGrid() {

}

bool MainGrid::validateAdd(Static* object) {
	IntVector2 size = object->getGridSize();
	Vector3* pos = object->getPosition();
	short posX = getIndex(pos->x_);
	short posZ = getIndex(pos->z_);

	for (int i = posX; i < posX + size.x_; ++i) {
		for (int j = posZ; j < posZ + size.y_; ++j) {
			const int index = i * resolution + j;
			if (!(inRange(index) &&
				buckets[index].getType() == ObjectType::UNIT)) {
				return false;
			}
		}
	}

	return true;
}

void MainGrid::addStatic(Static* object) {
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

void MainGrid::removeStatic(Static* object) {
	int index = object->getBucketIndex(0);
	buckets[index].removeStatic();
}

Vector3* MainGrid::validatePosition(Vector3* position) {
	short posX = getIndex(position->x_);
	short posZ = getIndex(position->z_);
	const int index = getIndex(posX, posZ);
	if (buckets[index].getType() != ObjectType::UNIT) {

		Vector3* direction = buckets[index].getDirectrionFrom(position);

		direction->Normalize();
		return direction;
	}
	return nullptr;
}
