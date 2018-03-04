#pragma once
#include "objects/Entity.h"
#include "database/db_strcut.h"
#include "objects/Static.h"
#include "objects/queue/QueueElement.h"
#include "objects/queue/QueueManager.h"

struct db_building;

class Building : public Static
{
public:
	Building(Vector3* _position, int id, int player, int level);
	~Building();

	float getHealthBarSize() override;
	int getDbID() override;
	void populate();
	void absorbAttack(double attackCoef) override;

	String& toMultiLineString() override;
	void action(short id, ActionParameter& parameter) override;
	void upgrade(char level);

	static std::string getColumns();
	std::string getValues(int precision) override;
	int getLevel() override;

	QueueElement* updateQueue(float time);
	Vector3& getTarget();
	QueueManager* getQueue();
private:
	Vector3 target;
	db_building* dbBuilding;
	db_building_level* dbLevel;

	std::vector<db_unit*>* units;
	QueueManager* queue;

	static double hbMaxSize;
};
