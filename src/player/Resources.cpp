#include "Resources.h"
#include "Game.h"
#include "database/DatabaseCache.h"


Resources::Resources() {
	Resources(0);
}

Resources::Resources(double valueForAll) {
	size = Game::get()->getDatabaseCache()->getResourceSize();

	for (int i = 0; i < size; ++i) {
		db_resource* resource = Game::get()->getDatabaseCache()->getResource(i);
		names[i] = new Urho3D::String(resource->name);
		values[i] = valueForAll;
	}
	changed = true;
}


Resources::~Resources() {
	delete names;
}

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

void Resources::add(int id, double value) {
	values[id] += value;
	changed = true;
}

bool Resources::hasChanged() {
	return changed;
}

short Resources::getSize() {
	return size;
}

double* Resources::getValues() {
	return values;
}

Urho3D::String** Resources::getNames() {
	return names;
}

void Resources::hasBeedUpdatedDrawn() {
	changed = false;
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
