#pragma once
#include "Player.h"
#include <vector>
#define TEAMS_NUMBER 5

class PlayersManager
{
public:
	PlayersManager();
	~PlayersManager();
	Player* getActivePlayer();
	Player* getPlayer(short i);
	std::vector<Player*>* getTeam(short i);
	void changeActive(short i);
private:
	Player* activePlayer;
	std::vector<Player*> allPlayers;
	std::vector<Player*>* teams[TEAMS_NUMBER];
};
