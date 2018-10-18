#include "UpgradeCommand.h"
#include "Game.h"
#include "objects/MenuAction.h"
#include "objects/building/Building.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/SimulationObjectManager.h"


UpgradeCommand::UpgradeCommand(int player, int id, MenuAction type): player(player), id(id), type(type) {
}

UpgradeCommand::~UpgradeCommand() = default;

void UpgradeCommand::execute() {
	char level = Game::getPlayersMan()->getPlayer(player)->upgradeLevel(type, id);
	if (type == MenuAction::BUILDING_LEVEL && level > 0) {
		for (auto building : *simulationObjectManager->getBuildings()) {
			if (building->getPlayer() == player && building->getDbID() == id) {
				building->upgrade(level);
			}
		}
	}
}

void UpgradeCommand::setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}
