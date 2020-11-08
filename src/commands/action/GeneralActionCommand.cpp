#include "GeneralActionCommand.h"

#include "Game.h"
#include "objects/queue/QueueActionType.h"
#include "GeneralActionType.h"
#include "database/db_strcut.h"
#include "player/Player.h"
#include "player/PlayersManager.h"

GeneralActionCommand::GeneralActionCommand(short id, GeneralActionType action, char player)
	: AbstractCommand(player),
	  id(id),
	  action(action) { }

void GeneralActionCommand::clean() {}

bool GeneralActionCommand::expired() {
	return false;
}

void GeneralActionCommand::execute() {
	if (action == GeneralActionType::BUILDING_LEVEL) {
		auto playerEnt = Game::getPlayersMan()->getPlayer(player);

		auto opt = playerEnt->getNextLevelForBuilding(id);//TODO ten id to powinien byc id levelu konkretnego
		if (opt.has_value()) {
			if (playerEnt->getResources().reduce(opt.value()->costs)) {
				playerEnt->getQueue().add(1, QueueActionType::BUILDING_LEVEL, id, 1);
			}
		}
	}
}
