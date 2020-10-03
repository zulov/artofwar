#pragma once
#include "Resources.h"
#include "Possession.h"
#include "objects/queue/QueueManager.h"
#include "ai/ActionMaker.h"
#include "ai/OrderMaker.h"

enum class ActionType : char;

class Player {
	friend class ActionMaker;
	friend class OrderMaker;
public:
	Player(int nationId, char team, char id, int color, Urho3D::String name, bool active);
	~Player();

	std::string getValues(int precision) const;
	void setResourceAmount(int resource, float amount);
	void setResourceAmount(float amount);
	char upgradeLevel(QueueActionType type, int id) const;

	Resources& getResources() { return resources; }
	Possession& getPossession() { return possession; }
	short getNation() const { return dbNation->id; }
	char getTeam() const { return team; }
	char getId() const { return id; }//TODO bug id playera a jego index to czêsto nie to samo
	int getColor() const { return color; }
	Urho3D::String& getName() { return name; }

	void updateResource() const;
	void updatePossession(SimulationInfo * simInfo);
	void add(Unit* unit);
	void add(Building* building);
	void ai();
	int getScore() const;
	int getAttackScore() const;
	int getDefenceScore() const;

	int getWorkersNumber() const;

	void deactivate();
	void activate();
	QueueElement* updateQueue(float time);
	QueueManager& getQueue();
	db_unit_level* getLevelForUnit(short id) const;
	db_building_level* getLevelForBuilding(short id) const;
	std::optional<db_unit_level*> getNextLevelForUnit(short id) const;
	std::optional<db_building_level*> getNextLevelForBuilding(short id) const;
private:
	Possession possession;
	Resources resources;
	QueueManager queue;
	ActionMaker actionMaker;
	OrderMaker orderMaker;
	db_nation* dbNation;
	Urho3D::String name;
	
	char team;
	char id;
	bool active;
	int color;

	char* unitLevels;
	char* buildingLevels;
};
