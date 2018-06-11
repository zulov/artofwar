#pragma once
#include "Physical.h"
#include "SurroundState.h"

class Static : public Physical
{
public:
	Static(Vector3* _position, ObjectType _type);
	virtual ~Static();

	void setBucketPosition(const IntVector2& _bucketPosition);
	static std::string getColumns();

	Vector2 getClosestCellPos(Vector3* pos) const;
	IntVector2& getBucketPosition() { return bucketPosition; }
	IntVector2& getGridSize() { return gridSize; }

	Vector2 getPosToFollow(Vector3* center) const override;
	std::string getValues(int precision) override;
protected:
	
	std::vector<int> ocupiedCells;
	std::vector<std::tuple<int, SurroundState, char>> surroundCells;

	IntVector2 gridSize;
	IntVector2 bucketPosition;
};
