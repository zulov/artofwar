#include "CreationCommandList.h"
#include "CreationCommand.h"
#include "Game.h"
#include "objects/ObjectEnums.h"
#include "database/DatabaseCache.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "simulation/SimulationObjectManager.h"
#include "simulation/env/Environment.h"
#include "player/Player.h"
#include "ActionCenter.h"


CreationCommandList::CreationCommandList(SimulationObjectManager* simulationObjectManager) {
	this->simulationObjectManager = simulationObjectManager;
}

CreationCommandList::~CreationCommandList() = default;

CreationCommand* CreationCommandList::addUnits(int number, int id, Urho3D::Vector2& position, char player, int level) {
	return new CreationCommand(ObjectType::UNIT, number, id, position, player, level);
}

CreationCommand* CreationCommandList::addBuilding(int id, Urho3D::Vector2& position, char player, int level) {
	Resources& resources = Game::getPlayersMan()->getPlayer(player)->getResources();
	auto costs = Game::getDatabaseCache()->getCostForBuilding(id);
	auto env = Game::getEnvironment();
	db_building* db_building = Game::getDatabaseCache()->getBuilding(id);

	if (env->validateStatic(db_building->size, position) && resources.reduce(costs)) {
		auto bucketCords = env->getBucketCords(db_building->size, position);
		auto pos = env->getValidPosition(db_building->size, position);

		return new CreationCommand(ObjectType::BUILDING, id, pos, player, bucketCords, level);
	}
	return nullptr;
}

CreationCommand* CreationCommandList::addResource(int id, Urho3D::Vector2& position, int level) {
	auto env = Game::getEnvironment();
	db_resource* db_resource = Game::getDatabaseCache()->getResource(id);

	if (env->validateStatic(db_resource->size, position)) {
		auto bucketCords = env->getBucketCords(db_resource->size, position);
		auto pos = env->getValidPosition(db_resource->size, position);

		return new CreationCommand(ObjectType::RESOURCE, id, pos, -1, bucketCords, level);
	}
	return nullptr;
}

void CreationCommandList::setParemeters(AbstractCommand* command) {
	dynamic_cast<CreationCommand*>(command)->setSimulationObjectManager(simulationObjectManager);
}
