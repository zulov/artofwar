#pragma once
#include "Physical.h"

class Static :public Physical
{
public:
	Static(Vector3* _position, Urho3D::Node* _node, ObjectType _type);
	~Static();
	IntVector2& getGridSize();
protected:

	IntVector2 gridSize;
};

