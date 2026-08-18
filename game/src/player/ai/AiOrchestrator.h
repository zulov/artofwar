#pragma once

#include <array>
#include <optional>
#include <span>
#include <utility>
#include <vector>

#include "WantList.h"
#include "WantExecutor.h"
#include "MasterBrain.h"
#include "EconomyBrain.h"
#include "UnitBrain.h"
#include "MilitaryBrain.h"
#include "MilitaryCommandCalculator.h"

class Physical;
class Player;
class Possession;
class Building;
class Unit;
class UnitOrder;
class AiHistory;

struct db_nation;
struct db_unit;
struct db_unit_level;
struct db_building;
struct db_building_level;
struct db_with_cost;

struct ResBuildingNeed {
	db_building* building;
	float need;
};

namespace Urho3D {
	class Vector2;
}

enum class ParentBuildingType : char;
enum class CenterType : char;

class AiOrchestrator {
public:
	explicit AiOrchestrator(Player* player, db_nation* nation, AiHistory* history);
	void createWorkers();
	void createUnits(const UnitOutput& unitOut, std::span<const float> unitProfileDiffs);
	void upgradeUnits(const UnitOutput& unitOut, std::span<const float> unitProfileDiffs);
	void upgradeWorkers();
	void upgradeUnitBuilding(const UnitOutput& unitOut, std::span<const float> unitProfileDiffs);

	void upgradeResBuilding(const std::vector<ResBuildingNeed>& buildingNeeds);
	void createResBuilding(const std::vector<ResBuildingNeed>& buildingNeeds);
	AiOrchestrator(const AiOrchestrator&) = delete;

	void action();
	void order();

private:

	// WantList request building (brain outputs -> wants)
	// One hop only: if the unit-like want cannot run because its producer is missing,
	// request that producer building and let the AI re-issue the original want next tick.
	void tryUnitWant(WantItemType type, float priority,unsigned short unitId, unsigned char count = 1);
	bool hasOwnedBuildingInstance(unsigned short buildingId) const;
	void submitBuildingRequest(float urgency, ParentBuildingType type);
	void submitBuildingUpgradeRequest(float urgency, ParentBuildingType type);
	short findBuildingToBuild(unsigned short unitId) const;
	bool hasAnyBuildingThatDeploy(unsigned short unitId) const;
	void tryToUpgradeBuilding(unsigned short id, float priority);

	// Army control (used by order())
	static constexpr float COMMAND_PRIORITY_DECAY_MULTIPLIER = 0.9f;
	static constexpr float COMMAND_PRIORITY_MULTIPLIER = 4.5f;
	static constexpr float MAX_COMMAND_PRIORITY = MAX_MILITARY_UNIT_PRESSURE * COMMAND_PRIORITY_MULTIPLIER;
	bool trySubmitUnitOrder(const std::vector<Unit*>& units, float priority, MilitaryCenterIdx center,
	                        UnitOrder* order) const;
	void decayUnitOrderPriorities() const;
	void issueAdvancePerUnit(const std::vector<std::pair<Unit*, float>>& units, MilitaryCenterIdx center,
	                         const Urho3D::Vector2& target);
	void issueHold(std::vector<std::pair<Unit*, MilitaryCenterIdx>>& group, float priority);
	bool tryIssueNearbyAttack(Unit* unit, float priority, MilitaryCenterIdx center) const;

	// Unit resolution
	std::vector<float> calculateUnitProfileDiffs(std::span<const float> unitProfile) const;
	std::vector<db_unit*> resolveUnit(const UnitOutput& unitOutput, std::span<const float> unitProfileDiffs);
	db_unit* resolveUnitUpgrade(std::span<const float> unitProfileDiffs);
	db_building* resolveBuildingUpgrade(std::span<const float> unitProfileDiffs);
	db_unit* resolveWorkerUpgrade();
	short resolveWorkerId() const;
	db_building* resolveResBuildingUpgrade(const std::vector<ResBuildingNeed>& buildingNeeds) const;

	// Building resolution
	db_building* resolveBuilding(ParentBuildingType type);
	std::vector<db_building*> getPossibleBuildingsInType(ParentBuildingType type) const;
	std::vector<ResBuildingNeed> calculateResBuildingNeeds() const;

	// Worker collection
	void manageWorkers();
	std::array<int, 4> computeWorkerTargets(const float (&prefs)[4], int workerCount) const;
	bool tryAssignCollect(Unit* worker, int resId);
	Unit* findReassignableWorker(const std::array<int, 4>& order, const float (&prefs)[4]) const;
	std::vector<Unit*> findFreeWorkers() const;
	Physical* closestInRange(Unit* worker, int resourceId);

	Player* player;
	unsigned char playerId;
	Possession* possession;
	db_nation* nation;
	AiHistory* history;

	MasterBrain masterBrain;
	EconomyBrain economyBrain;
	UnitBrain unitBrain;
	MilitaryBrain militaryBrain;
	MilitaryCommandCalculator militaryCommandCalculator;
	WantList wantList;
	WantExecutor wantExecutor;

	// TODO: Persist simulation time, AI history, and MasterBrain snapshot in saves; remove this warm-up then.
	bool skipFirstAiCycle = true;

	// Cached outputs
	MasterOutput lastMasterOut{};
	EconomyOutput lastEconOut{};
	MilitaryOutput lastMilOut{};
	WantList::LackingResult lastLacking{};
};
