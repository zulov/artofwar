#pragma once

#include <optional>
#include <vector>

#include "WantList.h"
#include "BuildSpatialBrain.h"

class Player;
class Possession;
class Building;
class AiHistory;

struct db_nation;
struct db_unit;
struct db_building;
struct MasterOutput;

namespace Urho3D {
	class Vector2;
}

enum class ParentBuildingType : char;
enum class CenterType : char;

// Carries out WantList items: turns an abstract want (worker/unit/building/upgrade)
// into concrete game actions. Holds all the low-level "how" so AiOrchestrator can
// stay focused on the high-level AI flow.
class WantExecutor : public IWantExecutor {
public:
	WantExecutor(Player* player, db_nation* nation, AiHistory* history);
	WantExecutor(const WantExecutor&) = delete;

	// Call once per tick before WantList::execute. Resets the lacking-building
	// feedback and caches the master-brain output used for placement decisions.
	void prepare(const MasterOutput& masterOut);

	// Building that was missing to produce/upgrade a wanted unit (-1 if none).
	short getLackingBuilding() const { return pendingLackingBuilding; }

	const db_with_cost* cost(const WantItem& item) const override;
	bool execute(WantItem& item) override;

private:
	bool executeWorker(short unitId);
	bool executeUnit(short unitId);
	bool executeBuilding(short buildingId);
	bool executeUnitUpgrade(short unitId);
	bool executeBuildingUpgrade(short buildingId);

	// Pick the ready building (that can produce 'unit') closest to the given center.
	Building* pickDeployBuilding(db_unit* unit, CenterType center) const;
	bool buildingProducesUnit(db_building* building, unsigned short unitId) const;
	std::vector<Building*> getBuildingsCanDeploy(unsigned short unitId) const;
	short findBuildingTypeToDeploy(short unitId) const;
	std::optional<Urho3D::Vector2> findPosToBuild(db_building* building, ParentBuildingType type);

	Player* player;
	unsigned char playerId;
	Possession* possession;
	db_nation* nation;
	AiHistory* history;

	BuildSpatialBrain buildSpatialBrain;
	const MasterOutput* masterOut = nullptr;
	short pendingLackingBuilding = -1;
};
