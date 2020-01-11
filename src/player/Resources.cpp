#include "Resources.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include <string>


Resources::Resources() {
	init(0);
}

void Resources::init(float valueForAll) {
	size = Game::getDatabaseCache()->getResourceSize();
	int i = 0;
	for (; i < size; ++i) {	
		values[i] = valueForAll;
	}
	for (; i < RESOURCE_NUMBER_DB; ++i) {
		values[i] = 0;
	}
	changed = true;
}

Resources::Resources(float valueForAll) {
	init(valueForAll);
}


Resources::~Resources() = default;

bool Resources::reduce(std::vector<db_cost*>* costs) {
	for (int i = 0; i < costs->size(); ++i) {
		const int j = costs->at(i)->resource;
		values[j] -= costs->at(i)->value;
		if (values[j] < 0) {
			revert(i, costs);
			return false;
		}
	}
	changed = true;
	return true;
}

void Resources::add(int id, float value) {
	values[id] += value;
	changed = true;
}

bool Resources::hasChanged() const {
	return changed;
}

short Resources::getSize() const {
	return size;
}

float* Resources::getValues() {
	return values;
}

void Resources::hasBeedUpdatedDrawn() {
	changed = false;
}

std::string Resources::getValues(int precision, int player) {
	std::string str;
	for (int i = 0; i < RESOURCE_NUMBER_DB; ++i) {
		str += "(" + std::to_string(player) + "," + std::to_string(i) + "," + std::to_string((int)(values[i] * precision)) + "),";
	}
	return str;
}

void Resources::setValue(int id, float amount) {
	values[id] = amount;
	changed = true;
}

std::string Resources::getColumns() {
	return "player		INT     NOT NULL,"
		"resource		INT     NOT NULL,"
		"amount		INT     NOT NULL";
}

void Resources::revert(int end, std::vector<db_cost*>* costs) {
	for (int i = 0; i < end + 1; ++i) {
		const int j = costs->at(i)->resource;
		values[j] += costs->at(i)->value;
	}
}
