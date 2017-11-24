#include "CreationCommandList.h"
#include "simulation/env/Enviroment.h"
#include "database/DatabaseCache.h"
#include "player/Resources.h"
#include "player/PlayersManager.h"
#include "Game.h"


CreationCommandList::CreationCommandList() {
	simulationObjectManager = new SimulationObjectManager();
}

CreationCommandList::~CreationCommandList() {
	delete simulationObjectManager;
}

bool CreationCommandList::addUnits(int _number, int id, Vector3* _position, int _player) {
	add(new CreationCommand(UNIT, _number, id, _position, _player));
	return true;
}

bool CreationCommandList::addBuilding(int id, Vector3* _position, int _player) {
	Resources* resources = Game::get()->getPlayersManager()->getActivePlayer()->getResources();
	std::vector<db_cost*>* costs = Game::get()->getDatabaseCache()->getCostForBuilding(id);
	Enviroment* env = Game::get()->getEnviroment();
	db_building* db_building = Game::get()->getDatabaseCache()->getBuilding(id);

	if (env->validateStatic(db_building->size, _position) && resources->reduce(costs)) {
		IntVector2 bucketCords = env->getBucketCords(db_building->size, _position);
		_position = env->getValidPosition(db_building->size, _position);

		add(new CreationCommand(BUILDING, id, _position, _player, bucketCords));
		return true;
	}
	return false;
}

bool CreationCommandList::addResource(int id, Vector3* _position) {
	Enviroment* env = Game::get()->getEnviroment();
	db_resource* db_resource = Game::get()->getDatabaseCache()->getResource(id);

	if (env->validateStatic(db_resource->size, _position)) {
		IntVector2 bucketCords = env->getBucketCords(db_resource->size, _position);
		_position = env->getValidPosition(db_resource->size, _position);

		add(new CreationCommand(RESOURCE, id, _position, -1, bucketCords));
		return true;
	}
	return false;
}

SimulationObjectManager* CreationCommandList::getManager() {
	return simulationObjectManager;
}

void CreationCommandList::setParemeters(AbstractCommand* command) {
	CreationCommand* m = static_cast<CreationCommand *>(command);
	m->setSimulationObjectManager(simulationObjectManager);
}
