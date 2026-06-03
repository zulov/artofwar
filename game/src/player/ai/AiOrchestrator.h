#pragma once

#include <optional>
#include <span>
#include <valarray>
#include <vector>

#include "WantList.h"
#include "MasterBrain.h"
#include "EconomyBrain.h"
#include "UnitBrain.h"
#include "MilitaryBrain.h"
#include "BuildSpatialBrain.h"
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
	// WantList execution callbacks
	bool executeWorker(short unitId);
	bool executeUnit(short unitId);
	bool executeBuilding(short buildingId);
	bool executeUnitUpgrade(short unitId);
	bool executeBuildingUpgrade(short buildingId);
	bool executeWantItem(WantItem& item);
	const db_with_cost* getWantItemCost(const WantItem& item) const;
	void submitBuildingRequest(float urgency, ParentBuildingType type);
	void submitBuildingUpgradeRequest(float urgency, ParentBuildingType type);

	// Unit resolution
	std::vector<db_unit*> resolveUnit(const UnitOutput& unitOutput);
	db_unit* resolveUnitUpgrade(const UnitOutput& unitOutput);
	db_building* resolveBuildingUpgrade(const UnitOutput& unitOutput);
	db_unit* resolveWorkerUpgrade();
	db_building* resolveResBuildingUpgrade(const EconomyOutput& econOutput);
	Building* getBuildingToDeploy(db_unit* unit) const;
	Building* getBuildingToDeployWorker(db_unit* unit) const;
	std::vector<Building*> getBuildingsCanDeploy(unsigned short unitId) const;
	short findBuildingTypeToDeploy(short unitId) const;

	// Building resolution
	db_building* resolveBuilding(ParentBuildingType type);
	std::optional<Urho3D::Vector2> findPosToBuild(db_building* building, ParentBuildingType type);
	std::vector<db_building*> getBuildingsInType(ParentBuildingType type);

	// Spatial helpers
	Building* getBuildingClosestArea(std::vector<Building*>& allPossible, std::span<const float> weights, int minAreas) const;

	// Distance matching
	float dist(std::valarray<float>& center, const db_basic_metric* metric);
	float dist(std::valarray<float>& center, const db_building_metric* metric, ParentBuildingType type);

	bool enoughResources(const db_with_cost* withCosts) const;

	// Worker collection
	void menageWorkers();
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
	BuildSpatialBrain buildSpatialBrain;
	AttackSpatialBrain attackSpatialBrain;

	WantList wantList;
	short pendingLackingBuilding = -1;

	// Cached outputs
	MasterOutput lastMasterOut{};
	EconomyOutput lastEconOut{};
	MilitaryOutput lastMilOut{};
	WantList::LackingResult lastLacking{};
};
