#include "Formation.h"
#include <algorithm>


Formation::Formation(short _id, std::vector<Unit*>* _units, FormationType _type) {
	id = _id;
	units = *_units;
	type = _type;
	for (auto unit : units) {
		unit->setFormation(id);
	}
}


Formation::~Formation() {
}

void Formation::update() {
	updateUnits();
	updateCenter();
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
