#pragma once
#include <unordered_set>
#include "../Physical.h"
#include "StaticState.h"
#include "simulation/env/influence/map/VisibilityType.h"

struct dbload_static;
enum class CellState : char;

class Static : public Physical {
public:
	Static(Urho3D::Vector3& _position, int indexInGrid, bool withNode);
	virtual ~Static();

	void setNextState(StaticState stateTo) { nextState = stateTo; }
	void setState(StaticState state) { this->state = state; }
	void load(dbload_static* dbloadStatic);

	int belowCloseLimit() const override;
	int hasFreeSpace() const;
	bool hasAnyFreeSpace() const;
	virtual bool canUse(int index) const =0;

	bool indexChanged() const override { return false; }
	StaticState getNextState() const { return nextState; }
	StaticState getState() const { return state; }
	bool isUsable() const override { return state == StaticState::ALIVE; }
	bool isAlive() const override { return state == StaticState::ALIVE || state == StaticState::FREE; }

	bool isToDispose() const override { return state == StaticState::DISPOSE; }
	virtual const Urho3D::IntVector2 getGridSize() const =0;
	// const std::span<int>& getOccupiedCells() const { return occupiedCells; }
	// const std::span<int>& getSurroundCells() const { return surroundCells; }
	const std::span<int> getOccupiedCells() const { return std::span{data, occupiedCellsSize}; }
	const std::span<int> getSurroundCells() const { return std::span{data + occupiedCellsSize, surroundCellsSize}; }

	std::optional<std::tuple<Urho3D::Vector2, float>> getPosToUseWithDist(Unit* user) override;
	std::vector<int> getIndexesForUse(Unit* user) const override;
	std::vector<int> getIndexesForRangeUse(Unit* user) const override;

	std::string getValues(int precision) override;
	unsigned short getIndexInInfluence() const { return indexInInfluence; }
	void setIndexInInfluence(int index);
	bool isInCloseRange(int index) const override;
	Urho3D::Color getColor(db_player_colors* col) const override;
	void setVisibility(VisibilityType type) override;
protected:
	void populate() override;
	float getHealthBarThick() const override { return 0.15f; }

	unsigned short indexInInfluence;
	StaticState state, nextState;

	unsigned char occupiedCellsSize = 0;
	unsigned char surroundCellsSize = 0;
	VisibilityType visibilityType = VisibilityType::VISIBLE;
	int* data{};
};
