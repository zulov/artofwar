#pragma once
#include "Resources.h"
#include "db_strcut.h"

class Player
{
public:
	Player(int nationId, int team);
	~Player();
private:
	Resources* resources;
	db_nation* dbNation;
	int team;
};
