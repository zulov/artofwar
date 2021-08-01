#pragma once
#include "Resources.h"
#include "Possession.h"
#include "objects/queue/QueueManager.h"
#include "ai/ActionMaker.h"
#include "ai/OrderMaker.h"
#include "database/db_strcut.h"

struct db_nation;
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
	char getId() const { return id; } //TODO bug id playera a jego index to czêsto nie to samo
	int getColor() const { return color; }
	Urho3D::String& getName() { return name; }

	void updateResource() const;
	void updatePossession(const ObjectsInfo* simInfo);
	void add(Unit* unit);
	void add(Building* building);
	void aiAction();
	void aiOrder();
	int getScore();

	int getWorkersNumber() const;

	void deactivate();
	void activate();
	QueueElement* updateQueue(float time) const;
	QueueManager* getQueue() const;
	db_unit_level* getLevelForUnit(short id) const;
	db_building_level* getLevelForBuilding(short id) const;
	std::optional<db_unit_level*> getNextLevelForUnit(short id) const;
	std::optional<db_building_level*> getNextLevelForBuilding(short id) const;
	void addKilled(Physical* physical);
	void resetScore();
private:
	db_nation* dbNation; //Must by first
	QueueManager* queue;
	Possession possession;
	Resources resources;
	ActionMaker actionMaker;
	OrderMaker orderMaker;
	Urho3D::String name;
	int score = -1;

	char team;
	char id;
	bool active;
	int color;

	char* unitLevels;
	char* buildingLevels;
};
