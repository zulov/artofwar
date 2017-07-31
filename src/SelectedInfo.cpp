#include "SelectedInfo.h"

SelectedInfo::SelectedInfo() {
	message = new Urho3D::String();
	allNumber = 0;

	lines = new Urho3D::String*[SELECTED_INFO_SIZE];
	hps = new double*[SELECTED_INFO_SIZE];
	for (int i = 0; i < SELECTED_INFO_SIZE; ++i) {
		hps[i] = new double[SELECTED_INFO_SIZE_2];
		for (int j = 0; j < SELECTED_INFO_SIZE_2; ++j) {
			hps[i][j] = -1;
		}
	}
	initLines();
}


SelectedInfo::~SelectedInfo() {
}

void SelectedInfo::initLines() {
	for (int i = 0; i < SELECTED_INFO_SIZE; ++i) {
		lines[i] = nullptr;
	}
}

void SelectedInfo::clearLines() {
	for (int i = 0; i < SELECTED_INFO_SIZE; ++i) {
		if (lines[i] != nullptr) {
			lines[i] = nullptr;
		}
	}

	for (int i = 0; i < SELECTED_INFO_SIZE; ++i) {
		for (int j = 0; j < SELECTED_INFO_SIZE_2; ++j) {
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

Urho3D::String** SelectedInfo::getLines() {
	clearLines();
	for (int i = 0; i < SELECTED_INFO_SIZE; ++i) {
		if (number[i] > 0) {
			lines[i] = new Urho3D::String(Urho3D::String(number[i]));
		} else {
			lines[i] = nullptr;
		}
		for (int j = 0; j < number[i]; ++j) {
			hps[i][j] = 1;
		}
	}

	return lines;
}

double** SelectedInfo::getHps() {
	for (int i = 0; i < SELECTED_INFO_SIZE; ++i) {
		for (int j = 0; j < number[i]; ++j) {
			hps[i][j] = 1;
		}
	}
	changed = false;
	return hps;
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

void SelectedInfo::setNumberAt(int type, int value) {
	number[type] = value;
	changed = true;
}

void SelectedInfo::incNumberAt(int type) {
	number[type]++;
	changed = true;
}

void SelectedInfo::reset() {
	changed = true;
	this->allNumber = 0;
	for (int i = 0; i < SELECTED_INFO_SIZE; ++i) {
		number[i] = 0;
	}
}
