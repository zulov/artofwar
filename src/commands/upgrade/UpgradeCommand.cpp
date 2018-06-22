#include "UpgradeCommand.h"
#include "Game.h"
#include "player/PlayersManager.h"
#include "objects/building/Building.h"
#include "simulation/SimulationObjectManager.h"
#include "objects/MenuAction.h"
#include "player/Player.h"

UpgradeCommand::UpgradeCommand(int _player, int _id, MenuAction _type) {
	player = _player;
	id = _id;
	type = _type;
}

UpgradeCommand::~UpgradeCommand() = default;

void UpgradeCommand::execute() {
	char level = Game::getPlayersManager()->getPlayer(player)->upgradeLevel(type, id);
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
