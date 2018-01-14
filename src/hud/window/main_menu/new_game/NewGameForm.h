#pragma once
#include "NewGamePlayer.h"

struct NewGameForm
{
	NewGamePlayer playerOne;
	NewGamePlayer enemy;
	int map;
	int difficulty;
	int gameSpeed;
};
