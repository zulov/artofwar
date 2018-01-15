#pragma once
#include "NewGamePlayer.h"

struct NewGameForm
{
	std::vector<NewGamePlayer> players;
	int map;
	int difficulty;
	int gameSpeed;
};
