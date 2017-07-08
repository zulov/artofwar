#pragma once
#include "Entity.h"
#include "BuildingType.h"
#include "Physical.h"

struct db_building;

class Building : public Physical {
public:
	Building(Vector3 *_position, Urho3D::Node* _node);
	~Building();
	double getHealthBarSize();
	int getSubType() override;
	void populate(db_building* dbBuilding);
	void absorbAttack(double attackCoef) override;
	bool isInGrandient();
	void setInGradinet(bool _inGradient);
private:
	BuildingType buildingType;
	bool inGradient = false;
};

