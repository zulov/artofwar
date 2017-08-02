#include "SelectedInfo.h"
#include "Controls.h"
#include "SelectedInfoType.h"

SelectedInfo::SelectedInfo() {
	message = new Urho3D::String();
	allNumber = 0;

	lines = new Urho3D::String*[MAX_SIZE_TYPES];
	hps = new double*[MAX_SIZE_TYPES];
	for (int i = 0; i < MAX_SIZE_TYPES; ++i) {
		hps[i] = new double[MAX_SELECTEDIN_TYPE];
		for (int j = 0; j < MAX_SELECTEDIN_TYPE; ++j) {
			hps[i][j] = -1;
		}
	}
	initLines();

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

void SelectedInfo::initLines() {
	for (int i = 0; i < MAX_SIZE_TYPES; ++i) {
		lines[i] = nullptr;
	}
}

void SelectedInfo::clearLines() {
	for (int i = 0; i < MAX_SIZE_TYPES; ++i) {
		if (lines[i] != nullptr) {
			lines[i] = nullptr;
		}
	}

	for (int i = 0; i < MAX_SIZE_TYPES; ++i) {
		for (int j = 0; j < MAX_SELECTEDIN_TYPE; ++j) {
			hps[i][j] = -1;
		}
	}
}

Urho3D::String* SelectedInfo::getMessage() {
	delete message;
	message = new Urho3D::String("Zaznaczeni: " + Urho3D::String(allNumber));
	changed = false;
	return message;
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
	if(entity->getSubType()>=0) {
		selectedByType[entity->getSubType()]->add(entity);
	}
	changed = true;
}
