#include "Possession.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "Resources.h"
#include <numeric>

int Possession::getScore() {
	return buildings.size() * 10 + units.size() + resourcesSum / 100;
}

void Possession::add(Building* building) {
	buildings.push_back(building);
}

void Possession::add(Unit* unit) {
	units.push_back(unit);
}

void Possession::updateAndClean(Resources& resources) {
	buildings.erase( //TODO performance iterowac tylko jezeli ktos umarl - przemyslec to
		std::remove_if(
			buildings.begin(), buildings.end(),
			[](Building* b) {
				return !b->isAlive();
			}
		),
		buildings.end());

	units.erase( //TODO performance iterowac tylko jezeli ktos umarl - przemyslec to
		std::remove_if(
			units.begin(), units.end(),
			[](Unit* u) {
				return !u->isAlive();
			}
		),
		units.end());
	short size = resources.getSize();
	float* values = resources.getValues();
	resourcesSum = std::accumulate(values, values + size, 0.f);
}
