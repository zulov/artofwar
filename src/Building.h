#pragma once
#include "Entity.h"

struct db_building;

class Building : public Entity {
public:
	Building(Vector3 *_position, Urho3D::Node* _boxNode, Font * _font);
	~Building();
	virtual int getType() override;
	void populate(db_building* dbBuilding);
};

