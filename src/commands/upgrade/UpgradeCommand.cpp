#include "UpgradeCommand.h"
#include "Game.h"
#include "objects/building/Building.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/SimulationObjectManager.h"


UpgradeCommand::UpgradeCommand(char player, int id, QueueActionType type): AbstractCommand(player), type(type), id(id) {
}

void UpgradeCommand::execute() {
	char level = Game::getPlayersMan()->getPlayer(player)->upgradeLevel(type, id);
	if (type == QueueActionType::BUILDING_LEVEL && level > 0) {
		for (auto building : *simulationObjectManager->getBuildings()) {
			if (building->getPlayer() == player && building->getId() == id) {
				building->levelUp(level);
			}
		}
	}
}

void UpgradeCommand::setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}
