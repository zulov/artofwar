#include "Formation.h"
#include "objects/unit/Unit.h"
#include <algorithm>
#include <iostream>


Formation::Formation(short _id, std::vector<Physical*>* _units, FormationType _type) {
	id = _id;
	for (auto value : *_units) {
		units.push_back(dynamic_cast<Unit*>(value));
	}
	type = _type;
	sideA = sqrt(units.size());
	sideB = units.size() / sideA;

	sizeA = (sideA - 1) * sparsity;
	sizeB = (sideB - 1) * sparsity;

	short k = 0;
	for (auto unit : units) {
		unit->setFormation(id);
		unit->setPositionInFormation(k++);
	}
	updateCenter();
}


Formation::~Formation() {
}

bool Formation::update() {
	updateUnits();
	if (!units.empty()) {
		updateCenter();
		return true;
	}
	return false;
}

Vector3 Formation::getPositionFor(short id) const {
	int column = id % sideA;
	int row = id / sideA;

	return center - Vector3(column * sparsity - sizeA / 2, 0, row * sparsity - sizeB / 2);
}

void Formation::updateUnits() {
	units.erase(
	            std::remove_if(
	                           units.begin(), units.end(),
	                           [this](Unit* unit)
	                           {
		                           return !unit->isAlive() || unit->getFormation() != id;
	                           }),
	            units.end());
}

void Formation::updateCenter() {
	center = Vector3::ZERO;
	for (auto unit : units) {
		if (unit->getPositionInFormation() < sideA * sideB) {
			center += *unit->getPosition();
		}
	}
	center /= units.size();
}
