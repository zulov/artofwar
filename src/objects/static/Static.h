#pragma once
#include "../Physical.h"
#include "StaticState.h"
#include "scene/load/dbload_container.h"

enum class CellState : char;

class Static : public Physical
{
public:
	Static(Urho3D::Vector3* _position, ObjectType _type, int mainCell);
	virtual ~Static();

	void setMainCell(int _mainCell);
	void setNextState(StaticState stateTo);
	void setState(StaticState state);
	void load(dbload_static* dbloadStatic);
	static std::string getColumns();

	int belowCloseLimit() override;
	int hasFreeSpace() const;
	static bool canCollect(int index);

	StaticState getNextState() const { return nextState; }
	StaticState getState() const { return state; }
	bool isUsable() const override { return state == StaticState::ALIVE; }
	bool isAlive() const override { return state == StaticState::ALIVE || state == StaticState::FREE ; }
	int getMainCell() const override { return mainCell; }
	bool isToDispose() const override {return state == StaticState::DISPOSE;}
	Urho3D::IntVector2& getGridSize() { return gridSize; }
	std::vector<int>& getOcupiedCells() { return occupiedCells; }
	std::vector<int>& getSurroundCells() { return surroundCells; }

	std::tuple<Urho3D::Vector2, int> getPosToFollowWithIndex(Urho3D::Vector3* center) const override;
	Urho3D::Vector2 getPosToFollow(Urho3D::Vector3* center) const override;
	std::string getValues(int precision) override;
protected:
	void populate(const Urho3D::IntVector2& size);

	std::vector<int> occupiedCells;
	std::vector<int> surroundCells;

	Urho3D::IntVector2 gridSize;
	int mainCell{};

	StaticState state;
	StaticState nextState;
};
