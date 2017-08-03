#include "SelectedInfo.h"
#include "Controls.h"
#include "SelectedInfoType.h"

SelectedInfo::SelectedInfo() {
	allNumber = 0;

	selectedByType = new SelectedInfoType*[MAX_SIZE_TYPES];
	for (int i = 0; i < MAX_SIZE_TYPES; ++i) {
		selectedByType[i] = new SelectedInfoType();
	}
}


SelectedInfo::~SelectedInfo() {
	for (int i = 0; i < MAX_SIZE_TYPES; ++i) {
		delete selectedByType[i];
	}
	delete[] selectedByType;
}

void SelectedInfo::clear() {
	
}

bool SelectedInfo::hasChanged() {
	return changed;
}

void SelectedInfo::setAllNumber(int allNumber) {
	this->allNumber = allNumber;
	changed = true;
}

void SelectedInfo::setSelectedType(ObjectType selectedType) {
	this->selectedType = selectedType;
	changed = true;
}

ObjectType SelectedInfo::getSelectedType() {
	return selectedType;
}

int SelectedInfo::getAllNumber() {
	return allNumber;
}

void SelectedInfo::reset() {
	changed = true;
	this->allNumber = 0;
	for (int i = 0; i < MAX_SIZE_TYPES; ++i) {
		selectedByType[i]->clear();
	}
}

SelectedInfoType** SelectedInfo::getSelecteType() {
	changed = false;
	return selectedByType;

}

void SelectedInfo::select(Physical* entity) {
	if (entity->getSubType() >= 0) {
		selectedByType[entity->getSubType()]->add(entity);
	}
	changed = true;
}
