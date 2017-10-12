#pragma once
#include "Physical.h"

class Static :public Physical
{
public:
	Static(Vector3* _position, ObjectType _type);
	~Static();
	IntVector2& getGridSize();
protected:
	Node * plane;
	IntVector2 gridSize;
};

