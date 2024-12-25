#pragma once
#include <unordered_set>
#include "../Physical.h"
#include "StaticState.h"
#include "env/influence/map/VisibilityType.h"

struct dbload_static;
enum class CellState : char;

class Static : public Physical {
public:
	Static(Urho3D::Vector3& _position, int indexInGrid, bool withNode, unsigned uId);
	~Static() override;

	void setNextState(StaticState stateTo) { nextState = stateTo; }
	void setState(StaticState state) { this->state = state; }
	void load(dbload_static* dbloadStatic);

	int belowCloseLimit() const override;
	int hasFreeSpace() const;
	bool hasAnyFreeSpace() const;
	virtual bool canUse(int index) const =0;

	StaticState getNextState() const { return nextState; }
	StaticState getState() const { return state; }
	bool isUsable() const override { return state == StaticState::ALIVE; }//TODO bug? moze inne stany tez? np do ataku?
	bool isAlive() const override { return state == StaticState::ALIVE || state == StaticState::FREE || state == StaticState::CREATING; }

	bool isToDispose() const override { return state == StaticState::DISPOSE; }
	virtual const Urho3D::IntVector2 getGridSize() const =0;

	const std::span<int> getOccupiedCells() const { return std::span{data, occupiedCellsSize}; }
	const std::span<int> getSurroundCells() const { return std::span{data + occupiedCellsSize, surroundCellsSize}; }
	const std::span<int> getAllCells() const { return std::span{data, static_cast<unsigned long>(occupiedCellsSize + surroundCellsSize) }; }

	std::optional<std::tuple<Urho3D::Vector2, float>> getPosToUseWithDist(Unit* user) override;
	std::vector<int> getIndexesForUse() const override;
	std::vector<int> getIndexesForRangeUse(Unit* user) const override;
	void addIndexesForUse(std::vector<int>& indexes) const override;
	bool indexCanBeUse(int index) const override;

	std::string getValues(int precision) override;

	void setBucketInMainGrid(int _bucketIndex) override {
		assert(_bucketIndex >= 0);
		assert(indexInMainGrid == -1);
		indexInMainGrid = _bucketIndex;
	}

	bool isInCloseRange(int index) const override;
	Urho3D::Color getColor(db_player_colors* col) const override;
	void setVisibility(VisibilityType type) override;
	Urho3D::IntVector2 getSurroundSize(Urho3D::IntVector2 oSize, int res);
	unsigned char getHealthBarThick() const override { return 6; }
protected:
	void populate() override;

	StaticState state = StaticState::CREATING;
	StaticState	nextState = StaticState::CREATING;
	
	unsigned char occupiedCellsSize = 0;
	unsigned char surroundCellsSize = 0;
	VisibilityType visibilityType = VisibilityType::VISIBLE;
	int* data{};
};
