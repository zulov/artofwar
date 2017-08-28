#include "Resources.h"
#include "Game.h"


Resources::Resources() {
	Resources(0);
}

Resources::Resources(float valueForAll) {
	size = Game::get()->getDatabaseCache()->getResourceSize();

	for (int i = 0; i < size; ++i) {
		db_resource* resource = Game::get()->getDatabaseCache()->getResource(i);
		names[i] = new String(resource->name);
		values[i] = valueForAll;
	}
	changed = true;
}


Resources::~Resources() {
	delete names;
}

bool Resources::reduce(std::vector<db_unit_cost*>* costs) {
	for (int i = 0; i < costs->size(); ++i) {
		int j = costs->at(i)->resource;
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

bool Resources::hasChanged() {
	return changed;
}

short Resources::getSize() {
	return size;
}

float* Resources::getValues() {
	return values;
}

Urho3D::String** Resources::getNames() {
	return names;
}

void Resources::hasBeedUpdatedDrawn() {
	changed = false;
}

void Resources::revert(int end, std::vector<db_unit_cost*>* costs) {
	for (int i = 0; i < end + 1; ++i) {
		int j = costs->at(i)->resource;
		values[j] += costs->at(i)->value;
	}
}
