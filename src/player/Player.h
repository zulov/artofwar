#pragma once
#include "Resources.h"
#include "Possession.h"
#include "ai/nn/Brain.h"
#include "objects/queue/QueueManager.h"
#include "stats/StatsEnums.h"

enum class ActionType : char;

class Player {
public:
	Player(int nationId, int team, char id, int color, Urho3D::String name, bool active);
	~Player();

	std::string getValues(int precision) const;
	void setResourceAmount(int resource, float amount);
	void setResourceAmount(float amount);
	char upgradeLevel(ActionType type, int id);

	Resources& getResources() { return resources; }
	Possession& getPossession() { return possession; }
	int getNation() const { return dbNation->id; }
	int getTeam() const { return team; }
	char getId() const { return id; }
	int getColor() const { return color; }
	Urho3D::String& getName() { return name; }
	char getLevelForUnit(int id) { return unitLevels[id]; }
	char getLevelForBuilding(int id) { return buildingLevels[id]; }
	char getLevelForUnitUpgradePath(short id) { return unitUpgradeLevels[id]; }
	char getLevelForUnitUpgrade(int id) { return unitUpgradeLevels[id]; }
	void updatePossession();
	void add(Unit* unit);
	void add(Building* building);
	void ai();
	int getScore() const;
	int getAttackScore() const;
	int getDefenceScore() const;
	int getUnitsNumber() const;
	int getBuildingsNumber() const;
	int getWorkersNumber() const;
	void deactivate();
	void activate();
	QueueElement* updateQueue(float time);
	QueueManager& getQueue();
private:

	short chooseUpgrade(StatsOutputType order);
	void createOrder(StatsOutputType order);
	Urho3D::Vector2 bestPosToBuild(const short id) const;
	void initAi();

	Brain* brain;
	Possession possession;
	Resources resources;
	QueueManager queue;
	db_nation* dbNation;
	Urho3D::String name;
	int team;
	int color;
	char id;
	bool active;
	char unitLevels[UNITS_NUMBER_DB];
	char buildingLevels[BUILDINGS_NUMBER_DB];
	char unitUpgradeLevels[PATH_UPGRADES_NUMBER_DB];
};
