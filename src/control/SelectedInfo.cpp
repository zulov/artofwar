#include "SelectedInfo.h"
#include "Controls.h"
#include "SelectedInfoType.h"

SelectedInfo::SelectedInfo() {
	allNumber = 0;
	allSubTypeNumber = 0;
	selectedByType = new vector<SelectedInfoType*>();
	selectedByType->reserve(MAX_SIZE_TYPES);

	for (int i = 0; i < MAX_SIZE_TYPES; ++i) {
		selectedByType->push_back(new SelectedInfoType());
	}
}

SelectedInfo::~SelectedInfo() {
	clear_vector(selectedByType);
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
	allNumber = 0;
	allSubTypeNumber = 0;
	for (int i = 0; i < selectedByType->size(); ++i) {
		selectedByType->at(i)->clear();
	}
}

vector<SelectedInfoType*>* SelectedInfo::getSelecteType() {
	return selectedByType;
}

void SelectedInfo::select(Physical* entity) {
	if (entity->getSubTypeId() >= 0) {
		selectedByType->at(entity->getSubTypeId())->add(entity);
		if (selectedByType->at(entity->getSubTypeId())->getData()->size() == 1) {
			++allSubTypeNumber;
		}
	}
	changed = true;
}

int SelectedInfo::getSelectedSubTypeNumber() {
	return allSubTypeNumber;
}

void SelectedInfo::hasBeedUpdatedDrawn() {
	changed = false;
}