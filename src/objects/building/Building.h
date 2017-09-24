#pragma once
#include "objects/Entity.h"
#include "BuildingType.h"
#include "database/db_strcut.h"
#include "QueueElement.h"
#include "QueueManager.h"
#include "objects/Static.h"

struct db_building;

class Building : public Static
{
public:
	Building(Vector3* _position, Urho3D::Node* _node);
	~Building();
	double getHealthBarSize() override;
	int getID() override;
	void populate(db_building* _dbBuilding, std::vector<db_unit*>* _units);
	void absorbAttack(double attackCoef) override;

	String* toMultiLineString() override;
	void action(short id, ActionParameter* parameter) override;
	QueueElement* updateQueue(float time);
	Vector3* getTarget();
	QueueManager* getQueue();
private:
	Vector3* target;
	BuildingType buildingType;
	db_building* dbBuilding;
	std::vector<db_unit*>* units;
	QueueManager* queue;


	static double hbMaxSize;
};
