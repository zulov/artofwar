#include "UpgradeCommand.h"
#include "UpgradeType.h"
#include "scene/load/RuntimeSaveData.h"
#include "Game.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/SimulationObjectManager.h"


UpgradeCommand::UpgradeCommand(unsigned char playerId, unsigned short id, UpgradeType type) :
	type(type), playerId(playerId), id(id) {}

void UpgradeCommand::execute(const SimulationObjectManager* simulationObjectManager) const {
	auto player = Game::getPlayersMan()->getPlayer(playerId);
	switch (type) {
	case UpgradeType::TECH_RESEARCH:
		player->completeTechnology(id);
		break;
	case UpgradeType::UNIT_LEVEL:
		player->upgradeUnitLevel(id);
		for (auto unit : *simulationObjectManager->getUnits()) {
			if (unit->getPlayer() == playerId && unit->getDbId() == id) {
				unit->levelUp();
			}
		}
		break;
	case UpgradeType::BUILDING_LEVEL:
		player->upgradeBuildingLevel(id);
		for (auto building : *simulationObjectManager->getBuildings()) {
			if (building->getPlayer() == playerId && building->getDbId() == id) {
				building->levelUp();
			}
		}
		break;
	default:
		return;
	}
	simulationObjectManager->refreshPlayerEffectiveLevels(playerId);
}

PendingCommandSaveData UpgradeCommand::saveState(unsigned short order) const {
	return {.order = order, .kind = PendingCommandKind::UPGRADE, .action = static_cast<char>(type), .actionType = 0,
	        .id = static_cast<unsigned short>(id), .playerId = playerId};
}
