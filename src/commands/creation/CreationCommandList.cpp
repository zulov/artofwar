#include "CreationCommandList.h"
#include "simulation/env/Enviroment.h"
#include "database/DatabaseCache.h"
#include "player/Resources.h"
#include "player/PlayersManager.h"
#include "Game.h"
#include "CreationCommand.h"


CreationCommandList::CreationCommandList() {
	simulationObjectManager = new SimulationObjectManager();
}

CreationCommandList::~CreationCommandList() {
	delete simulationObjectManager;
}

bool CreationCommandList::addUnits(int _number, int id, Vector3& _position, int _player) {
	add(new CreationCommand(UNIT, _number, id, new Vector3(_position), _player));
	return true;
}

bool CreationCommandList::addBuilding(int id, Vector3& _position, int _player) {
	Resources* resources = Game::get()->getPlayersManager()->getActivePlayer()->getResources();
	std::vector<db_cost*>* costs = Game::get()->getDatabaseCache()->getCostForBuilding(id);
	Enviroment* env = Game::get()->getEnviroment();
	db_building* db_building = Game::get()->getDatabaseCache()->getBuilding(id);

	if (env->validateStatic(db_building->size, _position) && resources->reduce(costs)) {
		Vector3 * pos = new Vector3(_position);
		IntVector2 bucketCords = env->getBucketCords(db_building->size, pos);
		pos = env->getValidPosition(db_building->size, pos);

		add(new CreationCommand(BUILDING, id, pos, _player, bucketCords));
		return true;
	}
	return false;
}

bool CreationCommandList::addResource(int id, Vector3& _position) {
	Enviroment* env = Game::get()->getEnviroment();
	db_resource* db_resource = Game::get()->getDatabaseCache()->getResource(id);

	if (env->validateStatic(db_resource->size, _position)) {
		Vector3 * pos = new Vector3(_position);
		IntVector2 bucketCords = env->getBucketCords(db_resource->size, pos);
		pos = env->getValidPosition(db_resource->size, pos);

		add(new CreationCommand(RESOURCE, id, pos, -1, bucketCords));
		return true;
	}
	return false;
}

SimulationObjectManager* CreationCommandList::getManager() {
	return simulationObjectManager;
}

void CreationCommandList::setParemeters(AbstractCommand* command) {
	CreationCommand* m = dynamic_cast<CreationCommand *>(command);
	m->setSimulationObjectManager(simulationObjectManager);
}
