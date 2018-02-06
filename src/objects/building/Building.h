#pragma once
#include "objects/Entity.h"
#include "database/db_strcut.h"
#include "QueueElement.h"
#include "QueueManager.h"
#include "objects/Static.h"

struct db_building;

class Building : public Static
{
public:
	Building(Vector3* _position, int id, int player);
	~Building();

	float getHealthBarSize() override;
	int getDbID() override;
	void populate(db_building* _dbBuilding, std::vector<db_unit*>* _units);
	void absorbAttack(double attackCoef) override;

	String& toMultiLineString() override;
	void action(short id, ActionParameter& parameter) override;
	static std::string getColumns();
	std::string getValues(int precision) override;
	QueueElement* updateQueue(float time);
	Vector3& getTarget();
	QueueManager* getQueue();
private:
	Vector3 target;
	db_building* dbBuilding;
	std::vector<db_unit*>* units;
	QueueManager* queue;


	static double hbMaxSize;
};
