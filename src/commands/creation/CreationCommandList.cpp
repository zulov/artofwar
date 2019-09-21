#include "CreationCommandList.h"
#include "CreationCommand.h"
#include "Game.h"
#include "ObjectEnums.h"
#include "database/DatabaseCache.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "simulation/SimulationObjectManager.h"
#include "simulation/env/Environment.h"
#include "player/Player.h"


CreationCommandList::CreationCommandList() {
	simulationObjectManager = new SimulationObjectManager();
}

CreationCommandList::~CreationCommandList() {
	delete simulationObjectManager;
}

bool CreationCommandList::addUnits(int _number, int id, Urho3D::Vector2& position, int player, int level) {
	add(new CreationCommand(ObjectType::UNIT, _number, id, position, player, level));
	return true;
}

bool CreationCommandList::addBuilding(int id, Urho3D::Vector2& position, int player, int level) {
	Resources& resources = Game::getPlayersMan()->getPlayer(player)->getResources();
	auto costs = Game::getDatabaseCache()->getCostForBuilding(id);
	auto env = Game::getEnvironment();
	db_building* db_building = Game::getDatabaseCache()->getBuilding(id);

	if (env->validateStatic(db_building->size, position) && resources.reduce(costs)) {
		auto bucketCords = env->getBucketCords(db_building->size, position);
		auto pos = env->getValidPosition(db_building->size, position);

		add(new CreationCommand(ObjectType::BUILDING, id, pos, player, bucketCords, level));
		return true;
	}
	return false;
}

bool CreationCommandList::addResource(int id, Urho3D::Vector2& position, int level) {
	auto env = Game::getEnvironment();
	db_resource* db_resource = Game::getDatabaseCache()->getResource(id);

	if (env->validateStatic(db_resource->size, position)) {
		auto bucketCords = env->getBucketCords(db_resource->size, position);
		auto pos = env->getValidPosition(db_resource->size, position);

		add(new CreationCommand(ObjectType::RESOURCE, id, pos, -1, bucketCords, level));
		return true;
	}
	return false;
}

SimulationObjectManager* CreationCommandList::getManager() const {
	return simulationObjectManager;
}

void CreationCommandList::setParemeters(AbstractCommand* command) {
	dynamic_cast<CreationCommand *>(command)->setSimulationObjectManager(simulationObjectManager);
}
