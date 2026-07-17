#pragma once

#include <array>
#include <optional>
#include <span>
#include <utility>
#include <valarray>
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
struct db_building_metric;
struct db_basic_metric;
struct db_with_cost;

namespace Urho3D {
	class Vector2;
}

enum class ParentBuildingType : char;
enum class CenterType : char;

class AiOrchestrator {
public:
	explicit AiOrchestrator(Player* player, db_nation* nation, AiHistory* history);
	void createWorkers();
	void createUnits(const UnitOutput& unitOut);
	void upgradeUnits(const UnitOutput& unitOut);
	void upgradeWorkers();
	void upgradeUnitBuilding(const UnitOutput& unitOut);
	void upgradeResBuilding();
	void createResBuilding();
	void createLackingUnitBuilding();
	AiOrchestrator(const AiOrchestrator&) = delete;

	void action();
	void order();

private:
	// WantList request building (brain outputs -> wants)
	void submitBuildingRequest(float urgency, ParentBuildingType type);
	void submitBuildingUpgradeRequest(float urgency, ParentBuildingType type);

	// Army control (used by order())
	static constexpr float COMMAND_PRIORITY_DECAY_MULTIPLIER = 0.9f;
	static constexpr float COMMAND_PRIORITY_MULTIPLIER = 4.5f;
	static constexpr float MAX_COMMAND_PRIORITY = MAX_MILITARY_UNIT_PRESSURE * COMMAND_PRIORITY_MULTIPLIER;
	bool trySubmitUnitOrder(const std::vector<Unit*>& units, float priority, MilitaryCenterIdx center,
	                        UnitOrder* order) const;
	bool trySubmitUnitOrder(Unit* unit, float priority, MilitaryCenterIdx center, UnitOrder* order) const;
	void decayUnitOrderPriorities() const;
	void issueAdvancePerUnit(const std::vector<std::pair<Unit*, float>>& units, MilitaryCenterIdx center,
	                         const Urho3D::Vector2& target);
	void issueHold(std::vector<std::pair<Unit*, MilitaryCenterIdx>>& group, float priority);

	// Unit resolution
	std::vector<db_unit*> resolveUnit(const UnitOutput& unitOutput);
	std::vector<float> unitsProfileMatch(const UnitOutput& unitOutput, std::vector<db_unit*> candidates);
	db_unit* resolveUnitUpgrade(const UnitOutput& unitOutput);
	db_building* resolveBuildingUpgrade(const UnitOutput& unitOutput);
	db_unit* resolveWorkerUpgrade();
	short resolveWorkerId() const;
	db_building* resolveResBuildingUpgrade(const EconomyOutput& econOutput) const;

	// Building resolution
	db_building* resolveBuilding(ParentBuildingType type);
	std::vector<db_building*> getBuildingsInType(ParentBuildingType type);

	// Distance matching
	float dist(std::valarray<float>& center, const db_basic_metric* metric);
	float dist(std::valarray<float>& center, const db_building_metric* metric, ParentBuildingType type);

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

	// Cached outputs
	MasterOutput lastMasterOut{};
	EconomyOutput lastEconOut{};
	MilitaryOutput lastMilOut{};
	WantList::LackingResult lastLacking{};
};
