#include "Formation.h"
#include <algorithm>
#include "objects/unit/Unit.h"

Formation::Formation(short _id, std::vector<Physical*>* _units, FormationType _type) {
	id = _id;
	for (auto value : *_units) {
		units.push_back(dynamic_cast<Unit*>(value));
	}
	type = _type;
	sideA = sqrt(units.size());
	sideB = units.size() / sideA;
	short k = 0;
	for (auto unit : units) {
		unit->setFormation(id);
		unit->setPositionInFormation(k++);
	}
}


Formation::~Formation() {
}

void Formation::update() {
	updateUnits();
	updateCenter();
}

Vector3 Formation::getPositionFor(short id) {
	int column = id % sideA;
	int row = id / sideA;
	return center - Vector3(column * sparsity, 0, row * sparsity);
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
		center += *unit->getPosition();
	}
	center /= units.size();
}
