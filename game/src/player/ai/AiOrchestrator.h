#pragma once

#include <array>
#include <optional>
#include <span>
#include <valarray>
#include <vector>

#include "WantList.h"
#include "WantExecutor.h"
#include "MasterBrain.h"
#include "EconomyBrain.h"
#include "UnitBrain.h"
#include "MilitaryBrain.h"
#include "AttackSpatialBrain.h"

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

class AiOrchestrator {
public:
	explicit AiOrchestrator(Player* player, db_nation* nation, AiHistory* history);
	AiOrchestrator(const AiOrchestrator&) = delete;

	void action();
	void order();

private:
	// WantList request building (brain outputs -> wants)
	void submitBuildingRequest(float urgency, ParentBuildingType type);
	void submitBuildingUpgradeRequest(float urgency, ParentBuildingType type);

	// Unit resolution
	std::vector<db_unit*> resolveUnit(const UnitOutput& unitOutput);
	db_unit* resolveUnitUpgrade(const UnitOutput& unitOutput);
	db_building* resolveBuildingUpgrade(const UnitOutput& unitOutput);
	db_unit* resolveWorkerUpgrade();
	short resolveWorkerId() const;
	db_building* resolveResBuildingUpgrade(const EconomyOutput& econOutput);

	// Building resolution
	db_building* resolveBuilding(ParentBuildingType type);
	std::vector<db_building*> getBuildingsInType(ParentBuildingType type);

	// Distance matching
	float dist(std::valarray<float>& center, const db_basic_metric* metric);
	float dist(std::valarray<float>& center, const db_building_metric* metric, ParentBuildingType type);

	// Worker collection
	void menageWorkers();
	std::array<int, 4> computeWorkerTargets(const float (&prefs)[4], int workerCount) const;
	bool tryAssignCollect(Unit* worker, int resId);
	Unit* findReassignableWorker(const std::array<int, 4>& order, const float (&prefs)[4]) const;
	std::vector<Unit*> findFreeWorkers() const;
	Physical* closestInRange(Unit* worker, int resourceId);
	UnitOrder* unitOrderCollect(std::vector<Unit*>& workers, Physical* closest) const;

	Player* player;
	unsigned char playerId;
	Possession* possession;
	db_nation* nation;
	AiHistory* history;

	MasterBrain masterBrain;
	EconomyBrain economyBrain;
	UnitBrain unitBrain;
	MilitaryBrain militaryBrain;
	AttackSpatialBrain attackSpatialBrain;

	WantList wantList;
	WantExecutor wantExecutor;

	// Cached outputs
	MasterOutput lastMasterOut{};
	EconomyOutput lastEconOut{};
	MilitaryOutput lastMilOut{};
	WantList::LackingResult lastLacking{};
};
