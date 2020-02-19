#pragma once
#include <vector>
#include "objects/unit/Unit.h"

void static cleanDead(std::vector<Unit*>& vector) {
	vector.erase(
		std::remove_if(
			vector.begin(), vector.end(),
			[](Unit* u) {
				return !u->isAlive();
			}
		),
		vector.end());
}

void static cleanDead(std::vector<Building*>& vector) {
	vector.erase(
		std::remove_if(
			vector.begin(), vector.end(),
			[](Building* u) {
				return !u->isAlive();
			}
		),
		vector.end());
}

void static cleanDead(std::vector<ResourceEntity*>& vector) {
	vector.erase(
		std::remove_if(
			vector.begin(), vector.end(),
			[](ResourceEntity* u) {
				return !u->isAlive();
			}
		),
		vector.end());
}
