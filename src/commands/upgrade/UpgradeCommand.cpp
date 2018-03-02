#include "UpgradeCommand.h"
#include "Game.h"
#include "player/PlayersManager.h"


UpgradeCommand::UpgradeCommand(int _player, int _id, QueueType _type) {
	player = _player;
	id = _id;
	type = _type;
}

UpgradeCommand::~UpgradeCommand() {
}

void UpgradeCommand::execute() {
	Game::get()->getPlayersManager()->getPlayer(player)->upgradeLevel(type, id);
}
