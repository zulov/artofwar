#include "CreationCommandList.h"
#include "CreationCommand.h"
#include "Game.h"
#include "objects/ObjectEnums.h"
#include "database/DatabaseCache.h"
#include "player/PlayersManager.h"
#include "simulation/env/Environment.h"
#include "player/Player.h"


CreationCommandList::CreationCommandList(SimulationObjectManager* simulationObjectManager) {
	this->simulationObjectManager = simulationObjectManager;
}

CreationCommand* CreationCommandList::addUnits(int number, int id, Urho3D::Vector2& position, char player, int level) const {
	return new CreationCommand(ObjectType::UNIT, number, id, position, player, level);
}

CreationCommand* CreationCommandList::addBuilding(int id, Urho3D::Vector2& position, char player, int level) const {
	Resources& resources = Game::getPlayersMan()->getPlayer(player)->getResources();
	db_building* building = Game::getDatabase()->getBuilding(id);
	if (resources.hasEnough(building->costs)) {
		const auto env = Game::getEnvironment();
		if (env->validateStatic(building->size, position)) {
			resources.reduce(building->costs);
			auto [bucketCords,pos] = env->getValidPosition(building->size, position);

			return new CreationCommand(ObjectType::BUILDING, id, pos, player, bucketCords, level);
		}
	}else {
		//TODO add resource presures
	}

	return nullptr;
}

CreationCommand* CreationCommandList::addResource(int id, Urho3D::Vector2& position, int level) const {
	auto env = Game::getEnvironment();
	db_resource* db_resource = Game::getDatabase()->getResource(id);

	if (env->validateStatic(db_resource->size, position)) {
		auto [bucketCords,pos] = env->getValidPosition(db_resource->size, position);

		return new CreationCommand(ObjectType::RESOURCE, id, pos, -1, bucketCords, level);
	}
	return nullptr;
}

void CreationCommandList::setParameters(AbstractCommand* command) {
	dynamic_cast<CreationCommand*>(command)->setSimulationObjectManager(simulationObjectManager);
}
