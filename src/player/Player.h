#pragma once
#include "Resources.h"
#include "ai/tree/AiNode.h"
#include "Possession.h"

enum class ActionType : char;

class Player {
public:
	Player(int nationId, int team, int id, int color, const Urho3D::String name, bool active);
	~Player();

	std::string getValues(int precision);
	void setResourceAmount(int resource, float amount);
	void setResourceAmount(float amount);
	char upgradeLevel(ActionType type, int id);

	Resources& getResources() { return resources; }
	int getNation() const { return dbNation->id; }
	int getTeam() const { return team; }
	int getId() const { return id; }
	int getColor() const { return color; }
	char getLevelForUnit(int id) { return unitLevels[id]; }
	char getLevelForBuilding(int id) { return buildingLevels[id]; }
	char getLevelForUnitUpgradePath(short id) { return unitUpgradeLevels[id]; }
	char getLevelForUnitUpgrade(int id) { return unitUpgradeLevels[id]; }
	void updatePossession();
	void add(Unit* unit);
	void add(Building* building);
	int getScore();
private:
	AiNode* aiRoot;
	Possession possession;
	Resources resources;
	db_nation* dbNation;
	Urho3D::String name;
	int team;
	int id;
	int color;
	bool active;
	char unitLevels[UNITS_NUMBER_DB];
	char buildingLevels[BUILDINGS_NUMBER_DB];
	char unitUpgradeLevels[PATH_UPGRADES_NUMBER_DB];
};
