#include "CreationCommandList.h"
#include "CreationCommand.h"
#include "Game.h"
#include "ObjectEnums.h"
#include "database/DatabaseCache.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "simulation/SimulationObjectManager.h"
#include "simulation/env/Enviroment.h"
#include "player/Player.h"


CreationCommandList::CreationCommandList() {
	simulationObjectManager = new SimulationObjectManager();
}

CreationCommandList::~CreationCommandList() {
	delete simulationObjectManager;
}

bool CreationCommandList::addUnits(int _number, int id, Urho3D::Vector2& _position, int _player, int level) {
	add(new CreationCommand(ObjectType::UNIT, _number, id, _position, _player, level));
	return true;
}

bool CreationCommandList::addBuilding(int id, Urho3D::Vector2& _position, int _player, int level) {
	Resources& resources = Game::getPlayersManager()->getActivePlayer()->getResources();
	auto costs = Game::getDatabaseCache()->getCostForBuilding(id);
	Enviroment* env = Game::getEnviroment();
	db_building* db_building = Game::getDatabaseCache()->getBuilding(id);

	if (env->validateStatic(db_building->size, _position) && resources.reduce(costs)) {

		auto bucketCords = env->getBucketCords(db_building->size, _position);
		auto pos = env->getValidPosition(db_building->size, _position);

		add(new CreationCommand(ObjectType::BUILDING, id, pos, _player, bucketCords, level));
		return true;
	}
	return false;
}

bool CreationCommandList::addResource(int id, Urho3D::Vector2& _position, int level) {
	Enviroment* env = Game::getEnviroment();
	db_resource* db_resource = Game::getDatabaseCache()->getResource(id);

	if (env->validateStatic(db_resource->size, _position)) {
		auto bucketCords = env->getBucketCords(db_resource->size, _position);
		auto pos = env->getValidPosition(db_resource->size, _position);

		add(new CreationCommand(ObjectType::RESOURCE, id, pos, -1, bucketCords, level));
		return true;
	}
	return false;
}

SimulationObjectManager* CreationCommandList::getManager() {
	return simulationObjectManager;
}

void CreationCommandList::setParemeters(AbstractCommand* command) {
	dynamic_cast<CreationCommand *>(command)->setSimulationObjectManager(simulationObjectManager);
}
