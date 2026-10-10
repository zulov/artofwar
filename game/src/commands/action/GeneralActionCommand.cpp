#include "GeneralActionCommand.h"
#include "scene/load/RuntimeSaveData.h"

#include "Game.h"
#include "objects/queue/QueueActionType.h"
#include "ActionTypes.h"
#include "database/db_struct.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "simulation/WorldAgeController.h"

GeneralActionCommand::GeneralActionCommand(unsigned short id, GeneralActionType action, unsigned char playerId)
	: id(id), action(action), playerId(playerId) {
}

void GeneralActionCommand::execute() {
	auto playerEnt = Game::getPlayersMan()->getPlayer(playerId);
	if (action == GeneralActionType::BUILDING_LEVEL) {
		auto opt = playerEnt->getNextBuildingLevel(id); //TODO ten id to powinien byc id levelu konkretnego
		if (opt.has_value()) {
			if (Game::getWorldAgeController()->isLevelAvailable(opt.value()->ageStage) &&
				playerEnt->getResources()->reduce(opt.value())) {
				playerEnt->getQueue().add(QueueActionType::BUILDING_LEVEL, id, opt.value()->id);
			}
		}
	}
}

PendingCommandSaveData GeneralActionCommand::saveState(unsigned short order) const {
	return {.order = order, .kind = PendingCommandKind::GENERAL_ACTION, .action = static_cast<char>(action),
	        .actionType = 0, .id = id, .playerId = playerId};
}
