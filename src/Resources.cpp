#include "Resources.h"
#include "Game.h"


Resources::Resources() {
	int size = Game::get()->getDatabaseCache()->getResourceSize();

	for (int i = 0; i < size; ++i) {
		db_resource* resource = Game::get()->getDatabaseCache()->getResource(i);
		names[i] = new String(resource->name);
	}
}


Resources::~Resources() {

}
