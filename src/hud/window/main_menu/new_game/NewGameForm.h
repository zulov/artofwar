#pragma once
#include "NewGamePlayer.h"

struct NewGameForm
{
	vector<NewGamePlayer> players;
	int map;
	int difficulty;
	int gameSpeed;
};
