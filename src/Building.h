#pragma once
#include "Entity.h"

class Building : public Entity {
public:
	Building(Vector3 _position, Urho3D::Node* _boxNode);
	~Building();
};

