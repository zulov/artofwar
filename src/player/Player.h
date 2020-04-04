#pragma once
#include "Resources.h"
#include "Possession.h"
#include "ai/nn/Brain.h"
#include "objects/queue/QueueManager.h"
#include "ai/ActionMaker.h"

enum class ActionType : char;

class Player {
	friend class ActionMaker;
public:
	Player(int nationId, char team, char id, int color, Urho3D::String name, bool active);
	~Player();

	std::string getValues(int precision) const;
	void setResourceAmount(int resource, float amount);
	void setResourceAmount(float amount);
	char upgradeLevel(QueueActionType type, int id);

	Resources& getResources() { return resources; }
	Possession& getPossession() { return possession; }
	int getNation() const { return dbNation->id; }
	char getTeam() const { return team; }
	char getId() const { return id; }
	int getColor() const { return color; }
	Urho3D::String& getName() { return name; }
	char getLevelForUnit(int id) { return unitLevels[id]; }
	char getLevelForBuilding(int id) { return buildingLevels[id]; }
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

	void initAi();

	Brain* brain;
	Possession possession;
	Resources resources;
	QueueManager queue;
	ActionMaker actionMaker;
	db_nation* dbNation;
	Urho3D::String name;
	char team;
	int color;
	char id;
	bool active;

	char* unitLevels;
	char* buildingLevels;
};
