#include "SelectedInfoType.h"
#include "objects/Physical.h"


SelectedInfoType::SelectedInfoType(unsigned short id):id(id) {
	data.reserve(10);
}

void SelectedInfoType::clear() {
	data.clear();
}

void SelectedInfoType::add(Physical* physical) {
	assert(physical->getDbId() == id);
	data.push_back(physical);
}
