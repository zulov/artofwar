#pragma once
#include "NewGamePlayer.h"
#include <vector>

struct NewGameForm
{
	std::vector<NewGamePlayer> players;
	int map;
	int difficulty;
	int gameSpeed;
};
