#pragma once
#include "../Physical.h"
#include "StaticState.h"

enum class CellState : char;

class Static : public Physical
{
public:
	Static(Urho3D::Vector3* _position, ObjectType _type, int mainCell);
	virtual ~Static();

	void setMainCell(int _mainCell);
	void setNextState(StaticState stateTo);
	void setState(StaticState state);
	static std::string getColumns();

	bool belowCloseLimit() override;
	bool hasFreeSpace() const;
	bool canCollect(int index, CellState type) const;

	StaticState getNextState() const { return nextState; }
	StaticState getState() const { return state; }
	bool isAlive() const override { return state == StaticState::ALIVE; }
	int getMainCell() const override { return mainCell; }
	Urho3D::IntVector2& getGridSize() { return gridSize; }
	std::vector<int>& getOcupiedCells() { return ocupiedCells; }
	std::vector<int>& getSurroundCells() { return surroundCells; }

	bool isToDispose() const override;
	std::tuple<Urho3D::Vector2, int> getPosToFollowWithIndex(Urho3D::Vector3* center) const override;
	Urho3D::Vector2 getPosToFollow(Urho3D::Vector3* center) const override;
	std::string getValues(int precision) override;
protected:
	void populate(const Urho3D::IntVector2& size);

	std::vector<int> ocupiedCells;
	std::vector<int> surroundCells;

	Urho3D::IntVector2 gridSize;
	int mainCell;

	StaticState state;
	StaticState nextState;
};