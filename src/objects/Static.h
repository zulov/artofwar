#pragma once
#include "Physical.h"
#include "StaticState.h"

enum class CellState : char;

class Static : public Physical
{
public:
	Static(Urho3D::Vector3* _position, ObjectType _type);
	virtual ~Static();

	void setMainCell(const Urho3D::IntVector2& _mainCell);
	static std::string getColumns();

	bool belowCloseLimit() override;
	bool hasFreeSpace() const;
	bool canCollect(int index, CellState type) const;

	Urho3D::IntVector2& getBucketPosition() { return mainCell; }
	Urho3D::IntVector2& getGridSize() { return gridSize; }
	std::vector<int>& getOcupiedCells() { return ocupiedCells; }
	std::vector<int>& getSurroundCells() { return surroundCells; }

	bool isToDispose() const override;
	Urho3D::Vector2 getPosToFollow(Urho3D::Vector3* center) const override;
	std::string getValues(int precision) override;
protected:
	void populate(const Urho3D::IntVector2& size);

	std::vector<int> ocupiedCells;
	std::vector<int> surroundCells;

	Urho3D::IntVector2 gridSize;
	Urho3D::IntVector2 mainCell;

	StaticState state;
};
