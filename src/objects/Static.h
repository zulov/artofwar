#pragma once
#include "Physical.h"

class Static : public Physical
{
public:
	Static(Vector3* _position, ObjectType _type);
	virtual ~Static();

	void setBucketPosition(const IntVector2& _bucketPosition);
	static std::string getColumns();

	IntVector2& getBucketPosition() { return bucketPosition; }
	IntVector2& getGridSize() { return gridSize; }
	Vector3 getClosestCellPos(Vector3* pos);

	std::string getValues(int precision) override;
protected:

	IntVector2 gridSize;
	IntVector2 bucketPosition;
};
