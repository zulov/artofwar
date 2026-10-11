#include "UpgradeCommand.h"
#include "scene/load/RuntimeSaveData.h"
#include "Game.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/SimulationObjectManager.h"
#include "objects/queue/QueueActionType.h"


UpgradeCommand::UpgradeCommand(unsigned char playerId, unsigned short id, QueueActionType type) :
	type(type), playerId(playerId), id(id) {}

void UpgradeCommand::execute(const SimulationObjectManager* simulationObjectManager) const {
	auto player = Game::getPlayersMan()->getPlayer(playerId);
	if (type == QueueActionType::TECH_RESEARCH && player->completeTechnology(id)) {
		simulationObjectManager->refreshPlayerEffectiveLevels(playerId);
	} else if (type == QueueActionType::UNIT_LEVEL && player->upgradeLevel(type, id) > 0) {
		for (auto unit : *simulationObjectManager->getUnits()) {
			if (unit->getPlayer() == playerId && unit->getDbId() == id)
				unit->levelUp();
		}
		simulationObjectManager->refreshPlayerEffectiveLevels(playerId);
	} else if (type == QueueActionType::BUILDING_LEVEL && player->upgradeLevel(type, id) > 0) {
		for (auto building : *simulationObjectManager->getBuildings()) {
			if (building->getPlayer() == playerId && building->getDbId() == id)
				building->levelUp();
		}
		simulationObjectManager->refreshPlayerEffectiveLevels(playerId);
	} else if (type == QueueActionType::UNIT_LEVEL || type == QueueActionType::BUILDING_LEVEL) {
		simulationObjectManager->refreshPlayerEffectiveLevels(playerId);
	}
}

PendingCommandSaveData UpgradeCommand::saveState(unsigned short order) const {
	return {.order = order, .kind = PendingCommandKind::UPGRADE, .action = static_cast<char>(type), .actionType = 0,
	        .id = static_cast<unsigned short>(id), .playerId = playerId};
}
