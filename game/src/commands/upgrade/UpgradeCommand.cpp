#include "UpgradeCommand.h"
#include "Game.h"
#include "objects/building/Building.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/SimulationObjectManager.h"
#include "objects/queue/QueueActionType.h"


UpgradeCommand::UpgradeCommand(char player, short id, QueueActionType type): player(player), type(type), id(id) {
}

void UpgradeCommand::execute(SimulationObjectManager* simulationObjectManager) const {
	char level = Game::getPlayersMan()->getPlayer(player)->upgradeLevel(type, id);
	if (type == QueueActionType::BUILDING_LEVEL && level > 0) {
		for (auto building : *simulationObjectManager->getBuildings()) {
			if (building->getPlayer() == player && building->getDbId() == id) {
				building->levelUp(level);
			}
		}
	}
}

void UpgradeCommand::setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}
