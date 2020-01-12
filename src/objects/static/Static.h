#pragma once
#include "../Physical.h"
#include "StaticState.h"
#include "scene/load/dbload_container.h"

enum class CellState : char;

class Static : public Physical
{
public:
	Static(Urho3D::Vector3& _position, int mainCell);
	virtual ~Static();

	void setMainCell(int _mainCell);
	void setNextState(StaticState stateTo);
	void setState(StaticState state);
	void load(dbload_static* dbloadStatic);
	static std::string getColumns();

	int belowCloseLimit() override;
	int hasFreeSpace() const;
	static bool canCollect(int index);

	bool isFirstThingInSameSocket() const override { return true; }
	StaticState getNextState() const { return nextState; }
	StaticState getState() const { return state; }
	bool isUsable() const override { return state == StaticState::ALIVE; }
	bool isAlive() const override { return state == StaticState::ALIVE || state == StaticState::FREE; }
	int getMainCell() const override { return mainCell; }
	bool isToDispose() const override { return state == StaticState::DISPOSE; }
	Urho3D::IntVector2& getGridSize() { return gridSize; }
	std::vector<int>& getOccupiedCells() { return occupiedCells; }
	std::vector<int>& getSurroundCells() { return surroundCells; }

	std::optional<std::tuple<Urho3D::Vector2, float, int>> getPosToUseWithIndex(Unit* unit) override;
	std::string getValues(int precision) override;
protected:
	void populate(const Urho3D::IntVector2& size);
	float getHealthBarThick() override { return 0.3; }
	float getShadowSize(const Urho3D::Vector3& boundingBox) const override;
	int mainCell{};
	StaticState state, nextState;
	Urho3D::IntVector2 gridSize;
	std::vector<int> occupiedCells, surroundCells;
};
