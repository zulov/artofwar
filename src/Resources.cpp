#include "Resources.h"
#include "Game.h"


Resources::Resources() {
	size = Game::get()->getDatabaseCache()->getResourceSize();

	for (int i = 0; i < size; ++i) {
		db_resource* resource = Game::get()->getDatabaseCache()->getResource(i);
		names[i] = new String(resource->name);
	}
}


Resources::~Resources() {

}

bool Resources::reduce(float* cost) {
	for (int i = 0; i < size; ++i) {
		values[i] -= cost[i];
		if(values[i]<0) {
			revert(i, cost);
			return false;
		}
	}
	return true;
}

void Resources::add(int id, float value) {
	values[id] += value;
}

void Resources::revert(int end, float* cost) {
	for (int i = 0; i < end + 1; ++i) {
		values[i] += cost[i];
	}
}
