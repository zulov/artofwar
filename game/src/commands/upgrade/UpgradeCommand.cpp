#include "UpgradeCommand.h"
#include "scene/load/RuntimeSaveData.h"
#include "Game.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/SimulationObjectManager.h"
#include "objects/queue/QueueActionType.h"


UpgradeCommand::UpgradeCommand(unsigned char playerId, short id, QueueActionType type, short levelId)
	: type(type), playerId(playerId), id(id), levelId(levelId) {
}

void UpgradeCommand::execute(SimulationObjectManager* simulationObjectManager) const {
	char level = type == QueueActionType::TECH_RESEARCH
		? Game::getPlayersMan()->getPlayer(playerId)->completeTechnology(levelId)
		: Game::getPlayersMan()->getPlayer(playerId)->upgradeLevel(type, id);
	if (type == QueueActionType::TECH_RESEARCH && level) {
		simulationObjectManager->refreshPlayerEffectiveLevels(playerId);
	} else if (type == QueueActionType::UNIT_LEVEL && level > 0) {
		for (auto unit : *simulationObjectManager->getUnits()) {
			if (unit->getPlayer() == playerId && unit->getDbId() == id) unit->levelUp();
		}
		simulationObjectManager->refreshPlayerEffectiveLevels(playerId);
	} else if (type == QueueActionType::BUILDING_LEVEL && level > 0) {
		for (auto building : *simulationObjectManager->getBuildings()) {
			if (building->getPlayer() == playerId && building->getDbId() == id) building->levelUp();
		}
		simulationObjectManager->refreshPlayerEffectiveLevels(playerId);
	} else if (type == QueueActionType::UNIT_LEVEL || type == QueueActionType::BUILDING_LEVEL) {
		simulationObjectManager->refreshPlayerEffectiveLevels(playerId);
	}
}

void UpgradeCommand::setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}

PendingCommandSaveData UpgradeCommand::saveState(unsigned short order) const {
	return {order, PendingCommandKind::UPGRADE, static_cast<char>(type), 0, static_cast<unsigned short>(id), playerId,
			static_cast<char>(levelId < 0 ? 0 : levelId)};
}
