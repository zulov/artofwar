#pragma once
#include <unordered_set>
#include "../Physical.h"
#include "StaticState.h"

struct dbload_static;
enum class CellState : char;

class Static : public Physical {
public:
	Static(Urho3D::Vector3& _position, int mainCell);
	virtual ~Static() = default;

	void setNextState(StaticState stateTo) { nextState = stateTo; }
	void setState(StaticState state) { this->state = state; }
	void load(dbload_static* dbloadStatic);
	void setSurroundCells(const std::unordered_set<int>& indexes);

	int belowCloseLimit() override;
	int hasFreeSpace() const;
	bool hasAnyFreeSpace() const;
	static bool canCollect(int index);

	bool isFirstThingInSameSocket() const override { return true; }
	StaticState getNextState() const { return nextState; }
	StaticState getState() const { return state; }
	bool isUsable() const override { return state == StaticState::ALIVE; }
	bool isAlive() const override { return state == StaticState::ALIVE || state == StaticState::FREE; }
	int getMainCell() const override { return mainCell; }
	bool isToDispose() const override { return state == StaticState::DISPOSE; }
	virtual const Urho3D::IntVector2 getGridSize() const =0;
	const std::vector<int>& getOccupiedCells() const { return occupiedCells; }
	const std::vector<int>& getSurroundCells() const { return surroundCells; }

	std::optional<std::tuple<Urho3D::Vector2, float, int>> getPosToUseWithIndex(Unit* unit) override;
	std::string getValues(int precision) override;
protected:
	void populate() override;
	float getHealthBarThick() const override { return 0.15f; }
	float getAuraSize(const Urho3D::Vector3& boundingBox) const override;
	int mainCell{};

	StaticState state, nextState;
	std::vector<int> occupiedCells, surroundCells;
};
