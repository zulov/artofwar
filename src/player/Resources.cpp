#include "Resources.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include <string>


Resources::Resources() {
	init(0);
}

void Resources::init(float valueForAll) {
	size = Game::getDatabase()->getResourceSize();
	values = new float[size];
	std::fill_n(values, size, valueForAll);

	changed = true;
}

Resources::Resources(float valueForAll) {
	init(valueForAll);
}


Resources::~Resources() {
	delete[] values;
};

bool Resources::reduce(const std::vector<db_cost*>& costs) {
	for (int i = 0; i < costs.size(); ++i) {
		const int j = costs.at(i)->resource;
		values[j] -= costs.at(i)->value;
		if (values[j] < 0) {
			revert(i, costs);
			return false;
		}
	}
	changed = true;
	return true;
}

bool Resources::hasEnough(const std::vector<db_cost*>& costs) const {
	for (auto cost : costs) {
		if (values[cost->resource] < cost->value) {
			return false;
		}
	}
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
	for (int i = 0; i < size; ++i) {
		str += "(" + std::to_string(player) + "," + std::to_string(i) + "," + std::to_string(
			(int)(values[i] * precision)) + "),";
	}
	return str;
}

void Resources::setValue(int id, float amount) {
	values[id] = amount;
	changed = true;
}

void Resources::change() {
	changed = true;
}

std::string Resources::getColumns() {
	return "player		INT     NOT NULL,"
		"resource		INT     NOT NULL,"
		"amount		INT     NOT NULL";
}

void Resources::revert(int end, const std::vector<db_cost*>& costs) {
	for (int i = 0; i < end + 1; ++i) {
		const int j = costs.at(i)->resource;
		values[j] += costs.at(i)->value;
	}
}
