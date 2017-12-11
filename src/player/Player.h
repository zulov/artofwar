#pragma once
#include "Resources.h"
#include "database/db_strcut.h"

class Player
{
public:
	Player(int nationId, int team, int _id);
	~Player();
	int getNation();
	int getTeam();
	int getId();
	Resources* getResources();
	std::string getValues(int precision);
	void setResourceAmount(int resource, float amount);
private:
	Resources* resources;
	db_nation* dbNation;
	int team;
	int id;
};
