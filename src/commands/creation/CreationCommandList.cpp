#include "CreationCommandList.h"
#include "CreationCommand.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "simulation/env/Enviroment.h"


CreationCommandList::CreationCommandList() {
	simulationObjectManager = new SimulationObjectManager();
}

CreationCommandList::~CreationCommandList() {
	delete simulationObjectManager;
}

bool CreationCommandList::addUnits(int _number, int id, Vector3& _position, int _player, int level) {
	add(new CreationCommand(ObjectType::UNIT, _number, id, new Vector3(_position), _player, level));
	return true;
}

bool CreationCommandList::addBuilding(int id, Vector2& _position, int _player, int level) {
	Resources& resources = Game::get()->getPlayersManager()->getActivePlayer()->getResources();
	std::vector<db_cost*>* costs = Game::get()->getDatabaseCache()->getCostForBuilding(id);
	Enviroment* env = Game::get()->getEnviroment();
	db_building* db_building = Game::get()->getDatabaseCache()->getBuilding(id);

	if (env->validateStatic(db_building->size, _position) && resources.reduce(costs)) {
		
		IntVector2 bucketCords = env->getBucketCords(db_building->size, _position);
		Vector3 * pos = env->getValidPosition(db_building->size, _position);

		add(new CreationCommand(ObjectType::BUILDING, id, pos, _player, bucketCords, level));
		return true;
	}
	return false;
}

bool CreationCommandList::addResource(int id, Vector2& _position, int level) {
	Enviroment* env = Game::get()->getEnviroment();
	db_resource* db_resource = Game::get()->getDatabaseCache()->getResource(id);

	if (env->validateStatic(db_resource->size, _position)) {
		IntVector2 bucketCords = env->getBucketCords(db_resource->size, _position);
		Vector3 * pos = env->getValidPosition(db_resource->size, _position);

		add(new CreationCommand(ObjectType::RESOURCE, id, pos, -1, bucketCords, level));
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
