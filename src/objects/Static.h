#pragma once
#include "Physical.h"

class Static :public Physical
{
public:
	Static(Vector3* _position, ObjectType _type);
	virtual ~Static();
	IntVector2& getBucketPosition();
	void setBucketPosition(const IntVector2& _bucketPosition);
	IntVector2& getGridSize();
	static std::string getColumns();
	std::string getValues(int precision) override;
protected:

	IntVector2 gridSize;
	IntVector2 bucketPosition;
};

