#pragma once
#include "Entity.h"
#include "BuildingType.h"

struct db_building;

class Building : public Entity {
public:
	Building(Vector3 *_position, Urho3D::Node* _node);
	~Building();
	ObjectType getType() override;
	int getSubType() override;
	void populate(db_building* dbBuilding);
	void absorbAttack(double attackCoef) override;
	bool isInGrandient();
	void setInGradinet(bool _inGradient);
private:
	BuildingType buildingType;
	bool inGradient = false;
};

