#include "CreationCommandList.h"
#include "CreationCommand.h"
#include "Game.h"
#include "objects/ObjectEnums.h"
#include "database/DatabaseCache.h"
#include "player/PlayersManager.h"
#include "env/Environment.h"
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
		if (env->isVisible(player, position)) {
			if (env->validateStatic(building->size, position, true)) {
				resources.reduce(building->costs);
				return new CreationCommand(ObjectType::BUILDING, id, env->getCords(position), level, player);
			}
		}
	} else {
		//TODO add resource presure
	}

	return nullptr;
}

CreationCommand* CreationCommandList::addResource(int id, Urho3D::Vector2& position) const {
	const auto env = Game::getEnvironment();
	const auto size = Game::getDatabase()->getResource(id)->size;

	if (env->validateStatic(size, position, false)) {
		return new CreationCommand(ObjectType::RESOURCE, id, env->getCords(position));
	}
	return nullptr;
}

CreationCommand*
CreationCommandList::addBuildingForce(int id, Urho3D::Vector2& position, char player, int level) const {
	db_building* building = Game::getDatabase()->getBuilding(id);
	const auto env = Game::getEnvironment();

	if (env->validateStatic(building->size, position, true)) {
		//resources.reduce(building->costs);
		return new CreationCommand(ObjectType::BUILDING, id, env->getCords(position), level, player);
	}

	return nullptr;
}

void CreationCommandList::setParameters(AbstractCommand* command) {
	dynamic_cast<CreationCommand*>(command)->setSimulationObjectManager(simulationObjectManager);
}
