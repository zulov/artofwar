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

static std::vector<short> intersection(std::vector<std::vector<short>*>& ids) {
	std::vector<short> common; //TODO check if ids sorted sometimes?
	if (ids.empty()) {
		return common;
	}
	common = *ids[0];
	for (int i = 1; i < ids.size(); ++i) {
		std::vector<short> temp;
		std::set_intersection(common.begin(), common.end(),
		                      ids[i]->begin(), ids[i]->end(),
		                      std::back_inserter(temp));
		common = temp; //TODO optimize, mo¿e nie kopiowaæ
		if (temp.empty()) {
			break;
		}
	}
	return common;
}
