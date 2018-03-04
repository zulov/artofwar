#pragma once
#include "Resources.h"
#include "database/db_strcut.h"
#include <list>
#include "ObjectEnums.h"
#include "objects/queue/QueueType.h"

class Player
{
public:
	Player(int nationId, int team, int _id, int _color, Urho3D::String _name, bool _active);
	~Player();
	int getNation();
	int getTeam();
	int getId();
	int getColor();
	Resources& getResources();
	std::string getValues(int precision);
	void setResourceAmount(int resource, float amount);
	void setResourceAmount(float amount);

	int getLevelForUnit(int id);
	int getLevelForBuilding(int id);
	char upgradeLevel(QueueType type, int id);
private:
	Resources resources;
	db_nation* dbNation;
	Urho3D::String name;
	int team;
	int id;
	int color;
	bool active;
	char unitLevels[UNITS_NUMBER_DB];
	char buildingLevels[BUILDINGS_NUMBER_DB];
};
