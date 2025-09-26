#include "CreationCommandList.h"
#include "CreationCommand.h"
#include "Game.h"
#include "objects/ObjectEnums.h"
#include "database/DatabaseCache.h"
#include "player/PlayersManager.h"
#include "env/Environment.h"
#include "player/Player.h"
#include "player/Resources.h"


CreationCommandList::CreationCommandList(SimulationObjectManager* simulationObjectManager)
	: simulationObjectManager(simulationObjectManager) {
}


CreationCommand* CreationCommandList::addUnits(unsigned number, short id, Urho3D::Vector2& position, char player,
                                               int level) const {
	return new CreationCommand(ObjectType::UNIT, id, position, level, player, number);
}

CreationCommand* CreationCommandList::addBuilding(short id, Urho3D::Vector2& position, char player, int level) const {
	Resources* resources = Game::getPlayersMan()->getPlayer(player)->getResources();
	db_building* building = Game::getDatabase()->getBuilding(id);
	if (resources->hasEnough(building->costs)) {
		const auto env = Game::getEnvironment();
		if (env->isVisible(player, position)) {
			const auto cords = env->getCords(position);
			if (env->validateStatic(building->size, cords, true)) {
				resources->reduce(building->costs);
				return new CreationCommand(ObjectType::BUILDING, id, cords, level, player);
			}
		}
	} else {
		//TODO add resource presure
	}

	return nullptr;
}

CreationCommand*
CreationCommandList::addBuildingForce(short id, Urho3D::Vector2& position, char player, int level) const {
	const db_building* building = Game::getDatabase()->getBuilding(id);
	const auto env = Game::getEnvironment();

	const auto cords = env->getCords(position);
	if (env->validateStatic(building->size, cords, true)) {
		return new CreationCommand(ObjectType::BUILDING, id, cords, level, player);
	}

	return nullptr;
}

CreationCommand* CreationCommandList::addResource(short id, const Urho3D::IntVector2& cords) const {
	const auto size = Game::getDatabase()->getResource(id)->size;

	if (Game::getEnvironment()->validateStatic(size, cords, false)) {
		return new CreationCommand(ObjectType::RESOURCE, id, cords);
	}
	return nullptr;
}

void CreationCommandList::add(CreationCommand* command) {
	commands.push_back(command);
}

void CreationCommandList::execute() {
	for (const auto command : commands) {
		command->execute(simulationObjectManager);
		delete command;
	}
	commands.clear();
}
