#include "SelectedInfoType.h"


SelectedInfoType::SelectedInfoType() {
	data = new std::vector<Physical*>();
	data->reserve(1000);
}


SelectedInfoType::~SelectedInfoType() {
}

void SelectedInfoType::clear() {
	data->clear();
}

void SelectedInfoType::add(Physical* physical) {
	data->push_back(physical);
}

std::vector<Physical*>* SelectedInfoType::getData() {
	return data;
}
