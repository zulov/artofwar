#include "UpgradeCommand.h"
#include "Game.h"
#include "player/PlayersManager.h"
#include "simulation/SimulationObjectManager.h"


UpgradeCommand::UpgradeCommand(int _player, int _id, ActionType _type) {
	player = _player;
	id = _id;
	type = _type;
}

UpgradeCommand::~UpgradeCommand() {
}

void UpgradeCommand::execute() {
	char level = Game::get()->getPlayersManager()->getPlayer(player)->upgradeLevel(type, id);
	if (type == ActionType::BUILDING_LEVEL) {
		if (level>0) {
			for (auto building : *simulationObjectManager->getBuildings()) {
				if (building->getPlayer() == player && building->getDbID() == id) {
					building->upgrade(level);
				}
			}
		}
	}
}

void UpgradeCommand::setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}
