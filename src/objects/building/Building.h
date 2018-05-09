#pragma once
#include "objects/Static.h"
#include "objects/queue/QueueElement.h"
#include "objects/queue/QueueManager.h"

struct db_building;
struct db_building_level;
struct db_unit;

class Building : public Static
{
public:

	Building(Vector3* _position, int id, int player, int level);
	~Building();

	float getMaxHpBarSize() override;
	int getDbID() override;
	void populate();
	void absorbAttack(float attackCoef) override;

	String& toMultiLineString() override;
	void action(char id, ActionParameter& parameter) override;
	void upgrade(char level);

	static std::string getColumns();
	std::string getValues(int precision) override;
	int getLevel() override;

	QueueElement* updateQueue(float time);
	Vector2& getTarget();
	QueueManager* getQueue();
private:
	Vector2 target;
	db_building* dbBuilding;
	db_building_level* dbLevel;

	std::vector<db_unit*>* units;
	QueueManager* queue;

};
