#pragma once
#include "Physical.h"

class Static :public Physical
{
public:
	Static(Vector3* _position, ObjectType _type);
	~Static();
	IntVector2& getBucketPosition();
	void setBucketPosition(IntVector2 _bucketPosition);
	IntVector2& getGridSize();
protected:

	IntVector2 gridSize;
	IntVector2 bucketPosition;
};

