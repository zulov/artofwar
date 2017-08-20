#pragma once
#include "Entity.h"
#include "BuildingType.h"
#include "Physical.h"
#include "db_strcut.h"
#include "QueueElement.h"

struct db_building;

class Building : public Physical
{
public:
	Building(Vector3* _position, Urho3D::Node* _node);
	~Building();
	double getHealthBarSize() override;
	int getSubType() override;
	int getSubTypeId() override;
	void populate(db_building* _dbBuilding, std::vector<db_unit*>* _units);
	void absorbAttack(double attackCoef) override;
	bool isInGrandient();
	void setInGradinet(bool _inGradient);
	String toMultiLineString() override;
	void buttonAction(short id) override;
private:
	BuildingType buildingType;
	db_building* dbBuilding;
	std::vector<db_unit*>* units;
	std::vector<QueueElement*> *queue;
	bool inGradient = false;
	static double hbMaxSize;
};
