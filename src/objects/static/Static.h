#pragma once
#include "../Physical.h"
#include "StaticState.h"
#include "database/db_strcut.h"
#include "scene/load/dbload_container.h"

enum class CellState : char;

class Static : public Physical {
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
	virtual const Urho3D::IntVector2 getGridSize() const =0;
	std::vector<int>& getOccupiedCells() { return occupiedCells; }
	std::vector<int>& getSurroundCells() { return surroundCells; }

	std::optional<std::tuple<Urho3D::Vector2, float, int>> getPosToUseWithIndex(Unit* unit) override;
	std::string getValues(int precision) override;
protected:
	void populate() override;
	float getHealthBarThick() const override { return 0.15; }
	float getAuraSize(const Urho3D::Vector3& boundingBox) const override;
	int mainCell{};

	StaticState state, nextState;
	std::vector<int> occupiedCells, surroundCells;
};
