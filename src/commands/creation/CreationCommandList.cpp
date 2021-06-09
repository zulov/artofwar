#include "CreationCommandList.h"
#include "CreationCommand.h"
#include "Game.h"
#include "objects/ObjectEnums.h"
#include "database/DatabaseCache.h"
#include "player/PlayersManager.h"
#include "simulation/env/Environment.h"
#include "player/Player.h"


CreationCommandList::CreationCommandList(SimulationObjectManager* simulationObjectManager)
	: simulationObjectManager(simulationObjectManager) {
}


CreationCommand* CreationCommandList::addUnits(int number, int id, Urho3D::Vector2& position, char player,
                                               int level) const {
	return new CreationCommand(ObjectType::UNIT, id, position, level, player, number);
}

CreationCommand* CreationCommandList::addBuilding(int id, Urho3D::Vector2& position, char player, int level) const {
	Resources& resources = Game::getPlayersMan()->getPlayer(player)->getResources();
	db_building* building = Game::getDatabase()->getBuilding(id);
	if (resources.hasEnough(building->costs)) {
		const auto env = Game::getEnvironment();
		if (env->validateStatic(building->size, position) && env->isVisible(position)) {
			resources.reduce(building->costs);
			return new CreationCommand(ObjectType::BUILDING, id, env->getCords(position), level, player);
		}
	} else {
		//TODO add resource presures
	}

	return nullptr;
}

CreationCommand* CreationCommandList::addResource(int id, Urho3D::Vector2& position, int level) const {
	auto env = Game::getEnvironment();
	db_resource* db_resource = Game::getDatabase()->getResource(id);

	if (env->validateStatic(db_resource->size, position)) {	
		return new CreationCommand(ObjectType::RESOURCE, id, env->getCords(position), level, -1);
	}
	return nullptr;
}

void CreationCommandList::setParameters(AbstractCommand* command) {
	dynamic_cast<CreationCommand*>(command)->setSimulationObjectManager(simulationObjectManager);
}
