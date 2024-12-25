#pragma once

#include "objects/queue/QueueManager.h"
#include "ai/ActionMaker.h"
#include "ai/OrderMaker.h"
#include "database/db_strcut.h"

class Possession;
class Resources;
struct db_nation;
enum class ActionType : char;

class Player {
	friend class ActionMaker;
	friend class OrderMaker;
public:
	Player(int nationId, char team, char id, int color, Urho3D::String name, bool active, unsigned currentUId);
	~Player();

	std::string getValues(int precision) const;
	void setResourceAmount(float food, float wood, float stone, float gold) const;
	void setResourceAmount(float amount) const;
	char upgradeLevel(QueueActionType type, int id) const;

	Resources* getResources() const { return resources; }
	Possession* getPossession() const { return possession; }
	short getNation() const { return dbNation->id; }
	char getTeam() const { return team; }
	char getId() const { return id; } //TODO bug id playera a jego index to czêsto nie to samo
	int getColor() const { return color; }
	Urho3D::String& getName() { return name; }

	void updateResource1s() const;
	void updateResourceMonth() const;
	void updateResourceYear() const;

	void updatePossession();
	void add(Unit* unit) const;
	void add(Building* building) const;
	void aiAction();
	void aiOrder();
	int getScore();

	int getWorkersNumber() const;

	QueueElement* updateQueue() const;
	QueueManager* getQueue() const;
	db_unit_level* getLevelForUnit(short id) const;
	db_building_level* getLevelForBuilding(short id) const;
	std::optional<db_unit_level*> getNextLevelForUnit(short id) const;
	std::optional<db_building_level*> getNextLevelForBuilding(short id) const;
	void addKilled(Physical* physical) const;
	void resetScore();
private:
	int score = -1;

	char team;
	char id;//przed possession,resources
	bool active;
	int color;
	unsigned currentBuildingUId;
	unsigned currentUnitUId;

	db_nation* dbNation; //Must by first
	QueueManager* queue;
	Possession* possession;
	Resources* resources;
	ActionMaker actionMaker;
	OrderMaker orderMaker;
	Urho3D::String name;

	char* unitLevels;
	char* buildingLevels;
};
