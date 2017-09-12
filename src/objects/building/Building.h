#pragma once
#include "objects/Entity.h"
#include "BuildingType.h"
#include "objects/Physical.h"
#include "database/db_strcut.h"
#include "QueueElement.h"
#include "QueueManager.h"

struct db_building;

class Building : public Physical
{
public:
	Building(Vector3* _position, Urho3D::Node* _node);
	~Building();
	double getHealthBarSize() override;
	int getSubTypeId() override;
	void populate(db_building* _dbBuilding, std::vector<db_unit*>* _units);
	void absorbAttack(double attackCoef) override;
	bool isInGrandient();
	void setInGradinet(bool _inGradient);
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
	bool inGradient = false;
	static double hbMaxSize;
};
