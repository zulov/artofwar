#include "GeneralActionCommand.h"

#include "Game.h"
#include "objects/queue/QueueActionType.h"
#include "GeneralActionType.h"
#include "database/db_strcut.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"

GeneralActionCommand::GeneralActionCommand(short id, GeneralActionType action, char player)
	: id(id), action(action), player(player) {
}

void GeneralActionCommand::execute() {
	if (action == GeneralActionType::BUILDING_LEVEL) {
		auto playerEnt = Game::getPlayersMan()->getPlayer(player);

		auto opt = playerEnt->getNextLevelForBuilding(id); //TODO ten id to powinien byc id levelu konkretnego
		if (opt.has_value()) {
			if (playerEnt->getResources()->reduce(opt.value())) {
				playerEnt->getQueue().add(1, QueueActionType::BUILDING_LEVEL, id);
			}
		}
	}
}
