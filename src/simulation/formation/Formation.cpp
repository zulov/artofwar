#include "Formation.h"
#include "objects/unit/Unit.h"
#include <algorithm>
#include <iostream>


Formation::Formation(short _id, std::vector<Physical*>* _units, FormationType _type, Vector2 _direction) : id(_id),
	type(_type), direction(_direction), state(FormationState::FORMING) {
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
	center = Vector2::ZERO;
	updateCenter();
}


Formation::~Formation() = default;

bool Formation::update() {
	updateUnits();
	if (!units.empty()) {
		updateCenter();
		return true;
	}
	return false;
}

Vector2 Formation::getPositionFor(short id) const {
	int column = id % sideA;
	int row = id / sideA;

	return center - Vector2(column * sparsity - sizeA / 2, row * sparsity - sizeB / 2);
}

float Formation::getPriority(int id) const {
	return notWellformed*;
}

float Formation::isReady() {
	return 0;
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
	//TODO need optimzation
	Vector2 temp = Vector2::ZERO;

	notWellformed = 0;
	float biggest = 0;
	for (auto unit : units) {
		if (unit->getPositionInFormation() < sideA * sideB) {
			temp.x_ += unit->getPosition()->x_;
			temp.y_ += unit->getPosition()->z_;
		}
		Vector2 pos = getPositionFor(unit->getPositionInFormation());
		float sth = Vector2(
		                    pos.x_ - unit->getPosition()->x_,
		                    pos.y_ - unit->getPosition()->z_
		                   ).LengthSquared();
		if (sth > biggest) {
			biggest = sth;
		}
		notWellformed += sth;
	}
	//biggest = sqrt(biggest);
	notWellformed = biggest + 1;
	notWellformed = Max(notWellformed, biggest);
	temp /= Min(units.size(), sideA * sideB);
	center = temp;
}
