#pragma once
#include <vector>
#include "objects/unit/Unit.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"

inline auto isAlive = [](Physical* u) {
	return !u->isAlive();
};

void static cleanDead(std::vector<Unit*>& vector) {
	vector.erase(
		std::remove_if(vector.begin(), vector.end(), isAlive),
		vector.end());
}

void static cleanDead(std::vector<Building*>& vector) {
	vector.erase(
		std::remove_if(vector.begin(), vector.end(), isAlive),
		vector.end());
}

void static cleanDead(std::vector<Building*>* vector) {
	vector->erase(
		std::remove_if(vector->begin(), vector->end(), isAlive),
		vector->end());
}

void static cleanDead(std::vector<ResourceEntity*>& vector) {
	vector.erase(
		std::remove_if(vector.begin(), vector.end(), isAlive),
		vector.end());
}
