#include "InfluenceMapInt.h"
#include <algorithm>

InfluenceMapInt::InfluenceMapInt(unsigned short size): InfluenceMap(size) {
	values = new unsigned char[size];
}

InfluenceMapInt::~InfluenceMapInt() {
	delete[] values;
}

void InfluenceMapInt::update(Physical* physical) {
	
	// const int index = indexFromPosition(unit->getPosition());
	//
	// if (!unit->isAlive()) {
	// 	removeAt(unit->getBucketIndex(team), unit);
	// } else if (unit->bucketHasChanged(index, team)) {
	// 	removeAt(unit->getBucketIndex(team), unit);
	// 	addAt(index, unit);
	// 	unit->setBucket(index, team);
	// }
}

void InfluenceMapInt::reset() {
	std::fill_n(values, size, 0);
}
