#include "AiOrchestrator.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <utility>
#include "AiHistory.h"
#include "AiUtils.h"
#include "NormScale.h"
#include "Game.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/BuildingActionType.h"
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "objects/building/Building.h"
#include "objects/building/ParentBuildingType.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Possession.h"
#include "player/Resources.h"
#include "player/ai/ActionCenter.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "objects/queue/QueueActionType.h"
#include "env/Environment.h"
#include "math/MathUtils.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/IndividualOrder.h"
#include "objects/unit/order/UnitOrder.h"
#include "objects/unit/order/UnitConst.h"
#include "objects/PhysicalUtils.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/state/UnitState.h"
#include "env/influence/CenterType.h"
#include "database/DatabaseCache.h"
#include "utils/PrintUtils.h"

namespace {
	constexpr unsigned int ECON_DEBUG_SECONDS = 20;

	bool isResBonus(db_building* b, db_building_level* l, ResourceType res) {
		return b->resourceType == cast(res) && l->collect > 0.f && l->resourceRange > 0.f;
	}

	constexpr float SEMI_CLOSE = 30.f;
	constexpr float SQ_SEMI_CLOSE = SEMI_CLOSE * SEMI_CLOSE;
	constexpr int MAX_RES_BUILDING_REQUESTS = 3;
	constexpr float MIN_ARMY_ORDER_PRESSURE = 0.1f;
	constexpr float MILITARY_COMMAND_RADIUS = 120.f;

	struct ArmyTargetSpec {
		MilitaryCenterIdx center;
		CenterType centerType;
		bool enemyOwner;
		AiOrderType orderType;
		// When true the target position is the center itself (taken from the
		// snapshot), not an enemy-facing attack area resolved via resolveAttackPos.
		bool moveToCenter;
	};

	constexpr ArmyTargetSpec ARMY_TARGET_SPECS[] = {
		{MilitaryCenterIdx::OUR_ARMY, CenterType::ARMY, false, AiOrderType::DEFEND_ARMY, true},
		{MilitaryCenterIdx::OUR_ECON, CenterType::ECON, false, AiOrderType::DEFEND_ECON, false},
		{MilitaryCenterIdx::OUR_BUILDING, CenterType::BUILDING, false, AiOrderType::DEFEND_BUILDING, false},
		{MilitaryCenterIdx::ENEMY_ARMY, CenterType::ARMY, true, AiOrderType::ATTACK_ARMY, false},
		{MilitaryCenterIdx::ENEMY_ECON, CenterType::ECON, true, AiOrderType::ATTACK_ECON, false},
		{MilitaryCenterIdx::ENEMY_BUILDING, CenterType::BUILDING, true, AiOrderType::ATTACK_BUILDING, false},
		{MilitaryCenterIdx::BATTLE, CenterType::BATTLE, false, AiOrderType::MOVE_BATTLE, true},
	};
	constexpr size_t ARMY_TARGET_SPEC_COUNT = sizeof(ARMY_TARGET_SPECS) / sizeof(ARMY_TARGET_SPECS[0]);

	constexpr size_t armyTargetIndex(MilitaryCenterIdx center) {
		for (size_t i = 0; i < ARMY_TARGET_SPEC_COUNT; ++i) {
			if (ARMY_TARGET_SPECS[i].center == center) { return i; }
		}
		return ARMY_TARGET_SPEC_COUNT;
	}
}


AiOrchestrator::AiOrchestrator(Player* player, db_nation* nation, AiHistory* history) :
	player(player), playerId(player->getId()), possession(player->getPossession()), nation(nation),
	history(history),
	masterBrain(nation),
	economyBrain(nation),
	unitBrain(nation),
	militaryBrain(nation),
	attackSpatialBrain(nation),
	militaryCommandCalculator(MILITARY_COMMAND_RADIUS),
	wantExecutor(player, nation, history) { lastLacking.reset(); }

void AiOrchestrator::createWorkers() {
	if (lastEconOut.workerCount > 0) {
		if (Game::getFrameInfo()->getSeconds() <= ECON_DEBUG_SECONDS) {
			PRINT("[ECON_WORKER_WANT]",
			      "sec", Game::getFrameInfo()->getSeconds(),
			      "player", playerId,
			      "priority", lastEconOut.workerAllocation,
			      "count", static_cast<int>(lastEconOut.workerCount),
			      "workerId", resolveWorkerId());
		}
		wantList.addRequest(WantItemType::WORKER, lastEconOut.workerAllocation, resolveWorkerId(),
		                    lastEconOut.workerCount);
	}
}

void AiOrchestrator::createUnits(const UnitOutput& unitOut) {
	if (unitOut.count > 0) {
		for (auto* unit : resolveUnit(unitOut)) {
			wantList.addRequest(WantItemType::UNIT, lastMasterOut.unitUrgency, unit->id);
		}
	}
}

void AiOrchestrator::upgradeUnits(const UnitOutput& unitOut) {
	if (unitOut.unitUpgradeUrgency > 0.1f) {
		if (auto* unitToUpgrade = resolveUnitUpgrade(unitOut)) {
			wantList.addRequest(WantItemType::UNIT_UPGRADE, unitOut.unitUpgradeUrgency, unitToUpgrade->id);
		}
	}
}

void AiOrchestrator::upgradeWorkers() {
	if (lastEconOut.workerUpgradeUrgency > 0.1f) {
		if (auto* workerToUpgrade = resolveWorkerUpgrade()) {
			wantList.addRequest(WantItemType::UNIT_UPGRADE, lastEconOut.workerUpgradeUrgency, workerToUpgrade->id);
		}
	}
}

void AiOrchestrator::upgradeUnitBuilding(const UnitOutput& unitOut) {
	if (unitOut.buildingUpgradeUrgency > 0.1f) {
		if (auto* buildingToUpgrade = resolveBuildingUpgrade(unitOut)) {
			wantList.addRequest(WantItemType::BUILDING_UPGRADE, unitOut.buildingUpgradeUrgency, buildingToUpgrade->id);
		}
	}
}

void AiOrchestrator::upgradeResBuilding() {
	if (lastEconOut.resBuildingUpgradeUrgency > 0.1f) {
		if (auto* resBuildingToUpgrade = resolveResBuildingUpgrade(lastEconOut)) {
			wantList.addRequest(WantItemType::BUILDING_UPGRADE, lastEconOut.resBuildingUpgradeUrgency, resBuildingToUpgrade->id);
		}
	}
}


void AiOrchestrator::createResBuilding() {
	struct Candidate {
		float need;
		unsigned short id;
	};
	std::vector<Candidate> candidates;

	for (const auto b : getBuildingsInType(ParentBuildingType::RESOURCE)) {
		auto* l = player->getLevelForBuilding(b->id);
		float need = 0.f;
		if (isResBonus(b, l, ResourceType::FOOD)) { need = std::max(need, lastEconOut.needBonusFood); }
		if (isResBonus(b, l, ResourceType::WOOD)) { need = std::max(need, lastEconOut.needBonusWood); }
		if (isResBonus(b, l, ResourceType::STONE)) { need = std::max(need, lastEconOut.needBonusStone); }
		if (isResBonus(b, l, ResourceType::GOLD)) { need = std::max(need, lastEconOut.needBonusGold); }
		if (b->toResource >= 0 && l->spawnResourceRange <= 0) { need = std::max(need, lastEconOut.needFoodSource); }
		if (b->toResource >= 0 && l->spawnResourceRange > 0) { need = std::max(need, lastEconOut.needWoodSource); }
		if (l->foodStorage > 0) { need = std::max(need, lastEconOut.needFoodStorage); }
		if (l->goldStorage > 0) { need = std::max(need, lastEconOut.needGoldStorage); }
		if (l->stoneRefineCapacity > 0.f) { need = std::max(need, lastEconOut.needStoneRefine); }
		if (l->goldRefineCapacity > 0.f) { need = std::max(need, lastEconOut.needGoldRefine); }
		if (need > 0.1f) { candidates.push_back({need, b->id}); }
	}

	const size_t keep = std::min<size_t>(candidates.size(), MAX_RES_BUILDING_REQUESTS);
	std::ranges::partial_sort(candidates, candidates.begin() + keep,
	                          [](const Candidate& a, const Candidate& b) { return a.need > b.need; });
	for (size_t i = 0; i < keep; ++i) {
		if (Game::getFrameInfo()->getSeconds() <= ECON_DEBUG_SECONDS) {
			PRINT("[ECON_BUILD_WANT]",
			      "sec", Game::getFrameInfo()->getSeconds(),
			      "player", playerId,
			      "buildingId", candidates[i].id,
			      "need", candidates[i].need);
		}
		wantList.addRequest(WantItemType::BUILDING, candidates[i].need, candidates[i].id);
	}
}

void AiOrchestrator::createLackingUnitBuilding() {
	if (lastLacking.lackingBuildingForUnit >= 0) {
		wantList.addRequest(WantItemType::BUILDING, std::max(lastMasterOut.unitUrgency, 0.5f),
		                    lastLacking.lackingBuildingForUnit);
	}
}

void AiOrchestrator::action() {
	const auto enemy = Game::getPlayersMan()->getEnemyFor(playerId);

	// 1. Master Brain decides urgencies
	lastMasterOut = masterBrain.decide(
			player, enemy,
			lastLacking.totalSum,
			history
			);

	// 2. Economy Brain (gets lacking feedback)
	float gameTime = norm(Game::getFrameInfo()->getSeconds(), NormScale::GAME_TIME_SHORT);
	lastEconOut = economyBrain.decide(
			player, enemy,
			lastLacking.perResource,
			lastMasterOut.economyUrgency, lastMasterOut.workerUrgency, lastMasterOut.expandUrgency,
			lastMasterOut.techUrgency, gameTime,
			history
			);

	// 3. Military Brain (composition prefs feed into UnitBrain)
	float techLevel = avgTechLevel(nation->units, nation->buildings, player);
	lastMilOut = militaryBrain.decide(
			player, enemy,
			lastMasterOut.militaryUrgency, lastMasterOut.attackUrgency,
			techLevel,
			history
			);

	// 4. Unit Brain
	auto unitOut = unitBrain.decide(
			player, enemy,
			lastMasterOut.unitUrgency, lastMasterOut.attackUrgency,
			lastMilOut.preferInfantry, lastMilOut.preferRange, lastMilOut.preferCavalry,
			lastMasterOut.techUrgency, gameTime
			);

	// 5. Submit requests to WantList
	wantList.resetRequests();

	createWorkers();
	upgradeWorkers();

	createUnits(unitOut);
	upgradeUnits(unitOut);

	// Unit-producing building upgrade request (barracks, archery range, stable)
	upgradeUnitBuilding(unitOut);

	// Resource building upgrade request (farms, mills, mines, refineries, etc.)
	upgradeResBuilding();

	// Defence building upgrade request (tower)
	submitBuildingUpgradeRequest(lastMasterOut.defenceBuildingUrgency, ParentBuildingType::DEFENCE);
	// Other building upgrade request (center, house)
	submitBuildingUpgradeRequest(lastMasterOut.buildingUrgency, ParentBuildingType::OTHER);
	// Tech building upgrade request (blacksmith, university)
	submitBuildingUpgradeRequest(lastMasterOut.techUrgency, ParentBuildingType::TECH);

	// Building requests — use MasterBrain urgencies directly
	submitBuildingRequest(lastMasterOut.defenceBuildingUrgency, ParentBuildingType::DEFENCE);
	submitBuildingRequest(lastMasterOut.buildingUrgency, ParentBuildingType::OTHER);
	submitBuildingRequest(lastMasterOut.techUrgency, ParentBuildingType::TECH);

	createResBuilding();

	createLackingUnitBuilding();

	// 6. Execute WantList
	wantExecutor.prepare(lastMasterOut);
	lastLacking = wantList.execute(player->getResources()->getValues(), wantExecutor);
	lastLacking.lackingBuildingForUnit = wantExecutor.getLackingBuilding();
}

//return first of its type
void AiOrchestrator::submitBuildingRequest(float urgency, ParentBuildingType type) {
	if (urgency > 0.1f) {
		if (db_building* building = resolveBuilding(type)) {
			wantList.addRequest(WantItemType::BUILDING, urgency, building->id);
		}
	}
}

void AiOrchestrator::submitBuildingUpgradeRequest(float urgency, ParentBuildingType type) {
	if (urgency <= 0.1f) { return; }
	for (auto* building : nation->buildings) {
		if (building->parentType[static_cast<int>(type)]
			&& player->getNextLevelForBuilding(building->id).has_value()) {
			wantList.addRequest(WantItemType::BUILDING_UPGRADE, urgency, building->id);
		}
	}
}

void AiOrchestrator::order() {
	manageWorkers();
	decayUnitOrderPriorities();

	std::vector<Unit*> allArmy = possession->getAllArmy();
	if (allArmy.empty()) { return; }

	const auto enemy = Game::getPlayersMan()->getEnemyFor(playerId);
	const auto enemyId = enemy->getId();
	std::array<std::optional<Urho3D::Vector2>, MILITARY_CENTER_COUNT> centers{};
	for (const auto& spec : ARMY_TARGET_SPECS) {
		const unsigned char owner = spec.enemyOwner ? enemyId : playerId;
		if (auto target = Game::getEnvironment()->getCenterOf(spec.centerType, owner)) {
			centers[castC(spec.center)] = *target;
		}
	}

	// Spatial brain output is target-independent, so compute it once and reuse it
	// for every target bucket below.
	const auto spatialOut = attackSpatialBrain.decide(
			player, enemy,
			lastMasterOut.militaryUrgency, lastMasterOut.attackUrgency);

	// Per-unit decision: each unit picks the highest-pressure target at its own
	// position. Units that fall below the pressure threshold (or pick a center
	// whose snapshot is unavailable) are routed to hold.
	std::array<std::vector<std::pair<Unit*, float>>, ARMY_TARGET_SPEC_COUNT> buckets{};
	std::vector<Unit*> holdUnits;
	for (auto* unit : allArmy) {
		const auto unitResult = militaryCommandCalculator.calculate(unit->getPosition(), centers, lastMilOut);
		const auto specIndex = armyTargetIndex(unitResult.best.center);
		if (unitResult.best.score <= MIN_ARMY_ORDER_PRESSURE || specIndex == ARMY_TARGET_SPEC_COUNT) {
			holdUnits.push_back(unit);
			continue;
		}
		buckets[specIndex].push_back({unit, unitResult.best.score});
	}

	// Resolve each non-empty bucket to a concrete target position (one resolve per
	// bucket, reusing the single spatialOut) and issue per-unit orders.
	for (size_t i = 0; i < ARMY_TARGET_SPEC_COUNT; ++i) {
		if (buckets[i].empty()) { continue; }
		const auto& spec = ARMY_TARGET_SPECS[i];
		const unsigned char owner = spec.enemyOwner ? enemyId : playerId;
		// Center position is FOW-independent (influence-map based), so it is
		// used as the fallback whenever a more precise target is unavailable.
		const auto& centerPos = centers[castC(spec.center)];
		// Move-to-center targets (e.g. OUR_ARMY, BATTLE) go straight to the
		// center position. Attack targets prefer an enemy-facing visible area, but
		// when that area is hidden by fog of war we still advance toward the
		// center position instead of holding.
		std::optional<Urho3D::Vector2> bestTarget;
		if (spec.moveToCenter) {
			bestTarget = centerPos;
		} else {
			bestTarget = resolveAttackPos(owner, spec.centerType, spatialOut);
			// TODO: the center-position fallback ignores fog of war — it sends units
			// toward an influence-map center that may be stale or unseen. Consider
			// factoring visibility back in here (e.g. only commit when the target is
			// visible/recently seen, or scout first) instead of advancing blindly.
			if (!bestTarget.has_value()) { bestTarget = centerPos; }
		}
		if (!bestTarget.has_value()) {
			for (const auto& entry : buckets[i]) { holdUnits.push_back(entry.first); }
			continue;
		}
		std::ranges::sort(buckets[i], [&](const auto& a, const auto& b) {
			return a.first->getPosition().SqDistXZ(*bestTarget) < b.first->getPosition().SqDistXZ(*bestTarget);
		});
		issueAdvancePerUnit(buckets[i], bestTarget.value());
		history->addOrder(spec.orderType, AiOrderResult::SUCCESS, static_cast<uint8_t>(buckets[i].size()));
	}

	if (!holdUnits.empty()) {
		history->addOrder(AiOrderType::NONE, AiOrderResult::NO_CENTER_POSITION);
		issueHold(holdUnits, MIN_ARMY_ORDER_PRESSURE);
	}
}

void AiOrchestrator::decayUnitOrderPriorities() const {
	for (auto* unit : possession->getAllArmy()) {
		unit->decayCommandPriority(COMMAND_PRIORITY_DECAY);
	}
}

bool AiOrchestrator::trySubmitUnitOrder(const std::vector<Unit*>& units, float priority, UnitOrder* order) const {
	for (auto* unit : units) {
		if (priority <= unit->getCommandPriority()) {
			delete order;
			return false;
		}
	}

	for (auto* unit : units) {
		unit->setCommandPriority(priority);
	}
	Game::getActionCenter()->addUnitAction(order);
	return true;
}

bool AiOrchestrator::trySubmitUnitOrder(Unit* unit, float priority, UnitOrder* order) const {
	return trySubmitUnitOrder(std::vector<Unit*>{unit}, priority, order);
}

std::optional<Urho3D::Vector2> AiOrchestrator::resolveAttackPos(unsigned char owner, CenterType fallbackType,
	                                                               const AttackSpatialOutput& spatialOut) {
	auto* env = Game::getEnvironment();
	const auto& areas = env->getBestVisibleAreas(playerId, std::span<const float>(spatialOut.weights));
	if (!areas.empty()) {
		// Only the best candidate is needed to position the army.
		// TODO consider using the remaining areas (e.g. fallback positions or
		// spreading sub-armies) instead of discarding them.
		return areas[0];
	}
	return env->getCenterOf(fallbackType, owner);
}

// Advance toward target per unit; each unit carries its own pressure score as its
// command priority. Far units march (GO); units already near the target attack the
// closest enemy. Orders are individual so each keeps its own priority.
void AiOrchestrator::issueAdvancePerUnit(const std::vector<std::pair<Unit*, float>>& units,
                                         const Urho3D::Vector2& target) {
	for (const auto& [unit, priority] : units) {
		if (priority <= unit->getCommandPriority()) { continue; }
		if (unit->getPosition().SqDistXZ(target) > SQ_SEMI_CLOSE) {
			trySubmitUnitOrder(unit, priority, new IndividualOrder(unit, UnitAction::GO, target));
		} else {
			auto& things = Game::getEnvironment()->getNeighboursFromTeamNotEq(unit, SEMI_CLOSE);
			if (!things.empty()) {
				const auto closest = Game::getEnvironment()->closestPhysical(
						unit->getMainGridIndex(), things, belowClose, true);
				if (closest) {
					trySubmitUnitOrder(unit, priority, new IndividualOrder(unit, UnitAction::ATTACK, closest));
				}
			}
		}
	}
}

// Hold — only free units engage nearby enemies
void AiOrchestrator::issueHold(std::vector<Unit*>& group, float priority) {
	for (auto* unit : group) {
		if (!isFree(unit)) { continue; }
		auto& things = Game::getEnvironment()->getNeighboursFromTeamNotEq(unit, SEMI_CLOSE);
		if (!things.empty()) {
			const auto closest = Game::getEnvironment()->closestPhysical(
					unit->getMainGridIndex(), things, belowClose, true);
			if (closest) {
				trySubmitUnitOrder(unit, priority, new IndividualOrder(unit, UnitAction::ATTACK, closest));
			}
		}
	}
}

// --- Unit resolution ---
// TODO: Consider producing more samples than 'count' (e.g. count + N extras) and storing
// the extras as fallback candidates in WantItem. When executeUnit() fails for the primary
// unit (no building), it could try the fallback before giving up. This avoids wasting a
// tick when the ideal unit is unbuildable but a similar one is available.

std::vector<db_unit*> AiOrchestrator::resolveUnit(const UnitOutput& unitOutput) {
	auto& units = nation->units;
	std::vector<db_unit*> candidates;
	candidates.reserve(units.size());
	for (auto unit : units) { if (!unit->typeWorker) { candidates.push_back(unit); } }
	if (candidates.empty()) { return {}; }

	std::vector<float> diffs = unitsProfileMatch(unitOutput, candidates);

	std::vector<db_unit*> result;
	result.reserve(unitOutput.count);
	for (const auto inx : lowestWithRand(diffs, unitOutput.count)) { result.push_back(candidates[inx]); }
	return result;
}

db_unit* AiOrchestrator::resolveUnitUpgrade(const UnitOutput& unitOutput) {
	auto& units = nation->units;
	std::vector<db_unit*> candidates;
	candidates.reserve(units.size());
	for (auto unit : units) {
		if (!unit->typeWorker && player->getNextLevelForUnit(unit->id).has_value()) { candidates.push_back(unit); }
	}
	if (candidates.empty()) { return nullptr; }

	std::vector<float> diffs = unitsProfileMatch(unitOutput, candidates);

	return candidates[lowestWithRand(diffs)];
}

std::vector<float> AiOrchestrator::unitsProfileMatch(const UnitOutput& unitOutput, std::vector<db_unit*> candidates) {
	std::valarray center(unitOutput.unitProfile.data(), unitOutput.unitProfile.size());
	std::vector<float> diffs;
	diffs.reserve(candidates.size());
	for (const auto unit : candidates) {
		diffs.push_back(dist(center, player->getLevelForUnit(unit->id)->dbUnitMetric));
	}
	return diffs;
}

db_building* AiOrchestrator::resolveBuildingUpgrade(const UnitOutput& unitOutput) {
	auto& buildings = nation->buildings;
	std::vector<db_building*> candidates;
	candidates.reserve(buildings.size());
	for (auto building : buildings) {
		if (building->parentType[static_cast<int>(ParentBuildingType::UNITS)]
			&& player->getNextLevelForBuilding(building->id).has_value()) { candidates.push_back(building); }
	}
	if (candidates.empty()) { return nullptr; }

	// Match building by which units it produces — find the building whose units
	// are closest to the desired unit profile
	std::valarray<float> center(unitOutput.unitProfile.data(), unitOutput.unitProfile.size());
	std::vector<float> diffs;
	diffs.reserve(candidates.size());
	for (const auto building : candidates) {
		// Average distance across all units this building can produce
		auto* level = player->getLevelForBuilding(building->id);
		float totalDist = 0.f;
		int unitCount = 0;
		for (auto* unit : level->allUnits) {
			if (!unit->typeWorker) {
				totalDist += dist(center, player->getLevelForUnit(unit->id)->dbUnitMetric);
				++unitCount;
			}
		}
		diffs.push_back(unitCount > 0 ? totalDist / static_cast<float>(unitCount) : std::numeric_limits<float>::max());
	}

	return candidates[lowestWithRand(diffs)];
}

db_unit* AiOrchestrator::resolveWorkerUpgrade() {
	for (auto* worker : nation->workers) { if (player->getNextLevelForUnit(worker->id).has_value()) { return worker; } }
	return nullptr;
}

// TODO: pick a worker type intentionally; for now just use the first one the nation has.
short AiOrchestrator::resolveWorkerId() const {
	return nation->workers.empty() ? -1 : nation->workers.at(0)->id;
}

db_building* AiOrchestrator::resolveResBuildingUpgrade(const EconomyOutput& econOutput) const {
	auto& buildings = nation->buildings;
	std::vector<db_building*> candidates;
	candidates.reserve(buildings.size());
	for (auto* building : buildings) {
		if (building->parentType[static_cast<int>(ParentBuildingType::RESOURCE)]
			&& player->getNextLevelForBuilding(building->id).has_value()) { candidates.push_back(building); }
	}
	if (candidates.empty()) { return nullptr; }
	if (candidates.size() == 1) { return candidates[0]; }

	// Weight by resource type priority + subtype need signals
	std::vector<float> weights;
	weights.reserve(candidates.size());
	for (const auto* building : candidates) {
		float weight = 0.1f; // base weight
		auto* level = player->getLevelForBuilding(building->id);

		// Resource type priority
		if (building->typeResourceFood) { weight += std::max(0.f, econOutput.foodPriority); }
		if (building->typeResourceWood) { weight += std::max(0.f, econOutput.woodPriority); }
		if (building->typeResourceStone) { weight += std::max(0.f, econOutput.stonePriority); }
		if (building->typeResourceGold) { weight += std::max(0.f, econOutput.goldPriority); }

		// Subtype need signals — distinguish between buildings of the same resource type
		if (level->foodStorage > 0) { weight += std::max(0.f, econOutput.needFoodStorage); }
		if (level->goldStorage > 0) { weight += std::max(0.f, econOutput.needGoldStorage); }
		if (level->stoneRefineCapacity > 0.f) { weight += std::max(0.f, econOutput.needStoneRefine); }
		if (level->goldRefineCapacity > 0.f) { weight += std::max(0.f, econOutput.needGoldRefine); }
		if (level->collect > 0.f && level->resourceRange > 0.f) {
			if (building->typeResourceFood) { weight += std::max(0.f, econOutput.needBonusFood); }
			if (building->typeResourceWood) { weight += std::max(0.f, econOutput.needBonusWood); }
			if (building->typeResourceStone) { weight += std::max(0.f, econOutput.needBonusStone); }
			if (building->typeResourceGold) { weight += std::max(0.f, econOutput.needBonusGold); }
		}
		if (building->toResource >= 0 && level->spawnResourceRange <= 0) {
			weight += std::max(0.f, econOutput.needFoodSource);
		}
		if (building->toResource >= 0 && level->spawnResourceRange > 0) {
			weight += std::max(0.f, econOutput.needWoodSource);
		}

		weights.push_back(weight);
	}

	float totalWeight = 0.f;
	for (float w : weights) { totalWeight += w; }
	return candidates[sampleWeighted(weights, totalWeight)];
}

// --- Building resolution ---

db_building* AiOrchestrator::resolveBuilding(ParentBuildingType type) {
	const auto buildings = getBuildingsInType(type);
	if (buildings.empty()) { return nullptr; }
	if (buildings.size() == 1) { return buildings.at(0); }

	// TODO: use building metric matching with brain output
	return buildings.at(0);
}

std::vector<db_building*> AiOrchestrator::getBuildingsInType(ParentBuildingType type) {
	std::vector<db_building*> buildings;
	buildings.reserve(nation->buildings.size());
	for (auto dbBuilding : nation->buildings) {
		if (dbBuilding->parentType[castC(type)]) { buildings.push_back(dbBuilding); }
	}
	return buildings;
}

float AiOrchestrator::dist(std::valarray<float>& center, const db_basic_metric* metric) {
	return sumSquaredError(center, metric->getValuesNormAsVal());
}

float AiOrchestrator::dist(std::valarray<float>& center, const db_building_metric* metric, ParentBuildingType type) {
	return sumSquaredError(center, metric->getValuesNormAsValForType(type));
}

// --- Worker collection ---

void AiOrchestrator::manageWorkers() {
	auto freeWorkers = findFreeWorkers();

	// Use economy brain's resource priorities to decide which resource to collect
	float prefs[] = {lastEconOut.foodPriority, lastEconOut.woodPriority,
	                 lastEconOut.stonePriority, lastEconOut.goldPriority};
	std::array order = {0, 1, 2, 3};
	std::ranges::sort(order, [&](int a, int b) { return prefs[a] > prefs[b]; });

	if (Game::getFrameInfo()->getSeconds() <= ECON_DEBUG_SECONDS) {
		PRINT("[ECON_ASSIGN]",
		      "sec", Game::getFrameInfo()->getSeconds(),
		      "player", playerId,
		      "prefs", prefs[0], prefs[1], prefs[2], prefs[3],
		      "order", order[0], order[1], order[2], order[3],
		      "freeWorkers", static_cast<int>(freeWorkers.size()));
	}

	// Reassign one busy worker away from the most negative resource.
	if (auto* worker = findReassignableWorker(order, prefs)) {
		if (Game::getFrameInfo()->getSeconds() <= ECON_DEBUG_SECONDS) {
			auto* res = dynamic_cast<ResourceEntity*>(worker->getThingToInteract());
			PRINT("[ECON_REASSIGN]",
			      "sec", Game::getFrameInfo()->getSeconds(),
			      "player", playerId,
			      "worker", worker->getSecondaryId(),
			      "fromRes", res ? static_cast<int>(res->getResourceId()) : -1,
			      "worstPref", prefs[order[3]]);
		}
		freeWorkers.push_back(worker);
	}

	if (freeWorkers.empty()) { return; }

	// Split workers across resources proportionally to their needs.
	std::array<int, RESOURCES_SIZE> remaining = computeWorkerTargets(prefs, freeWorkers.size());
	if (Game::getFrameInfo()->getSeconds() <= ECON_DEBUG_SECONDS) {
		PRINT("[ECON_TARGETS]",
		      "sec", Game::getFrameInfo()->getSeconds(),
		      "player", playerId,
		      "targets", remaining[0], remaining[1], remaining[2], remaining[3]);
	}

	for (auto* worker : freeWorkers) {
		bool assigned = false;
		// 1. Honour the quota: highest-priority resource that still needs workers and is reachable.
		for (int resId : order) {
			if (remaining[resId] <= 0) { continue; }
			if (tryAssignCollect(worker, resId)) {
				--remaining[resId];
				assigned = true;
				break;
			}
		}
		// 2. Fallback: quota exhausted or unreachable — take any reachable resource by priority.
		if (!assigned) {
			for (int resId : order) {
				if (tryAssignCollect(worker, resId)) {
					assigned = true;
					break;
				}
			}
		}
		if (!assigned) {
			auto failType = static_cast<AiOrderType>(
				static_cast<uint8_t>(AiOrderType::COLLECT_RESOURCE_0) + order[0]);
			history->addOrder(failType, AiOrderResult::NO_RESOURCE_IN_RANGE, 1);
		}
	}
}

// Distributes workerCount workers across the 4 resource types proportionally to their
// remapped priorities, using largest-remainder rounding so the targets sum to workerCount.
std::array<int, 4> AiOrchestrator::computeWorkerTargets(const float (&prefs)[4], int workerCount) const {
	std::array target = {0, 0, 0, 0};
	const std::array<float, 4> weights = {
		priorityWeight(prefs[0]),
		priorityWeight(prefs[1]),
		priorityWeight(prefs[2]),
		priorityWeight(prefs[3]),
	};

	float total = 0.f;
	for (float weight : weights) { total += weight; }
	if (total <= 0.f || workerCount <= 0) { return target; }

	// Floor each share, remembering the fractional remainder for the tie-break below.
	float frac[4];
	int leftover = workerCount;
	for (int i = 0; i < 4; ++i) {
		const float raw = weights[i] / total * static_cast<float>(workerCount);
		target[i] = static_cast<int>(std::floor(raw));
		frac[i] = raw - static_cast<float>(target[i]);
		leftover -= target[i];
	}

	// Hand out the remaining workers to the largest fractional parts.
	for (; leftover > 0; --leftover) {
		const int best = static_cast<int>(std::ranges::max_element(frac) - frac);
		++target[best];
		frac[best] = -1.f;
	}
	return target;
}

// Issues a COLLECT order for the nearest reachable node of the given resource type.
// Returns false if no node is in range.
bool AiOrchestrator::tryAssignCollect(Unit* worker, int resId) {
	auto* closest = closestInRange(worker, resId);
	if (!closest) { return false; }
	auto orderType = static_cast<AiOrderType>(static_cast<uint8_t>(AiOrderType::COLLECT_RESOURCE_0) + resId);
	if (Game::getFrameInfo()->getSeconds() <= ECON_DEBUG_SECONDS) {
		PRINT("[ECON_COLLECT]",
		      "sec", Game::getFrameInfo()->getSeconds(),
		      "player", playerId,
		      "worker", worker->getSecondaryId(),
		      "res", resId,
		      "target", closest->getSecondaryId());
	}
	Game::getActionCenter()->addUnitAction(new IndividualOrder(worker, UnitAction::COLLECT, closest));
	history->addOrder(orderType, AiOrderResult::SUCCESS, 1);
	return true;
}

// Returns a busy worker collecting the most negative resource when reassignment is forced.
Unit* AiOrchestrator::findReassignableWorker(const std::array<int, 4>& order, const float (&prefs)[4]) const {
	int worstResId = order[3];
	if (prefs[worstResId] >= -0.5f) { return nullptr; }
	for (auto* worker : possession->getWorkers()) {
		if (worker->getState() != UnitState::COLLECT) { continue; }
		auto* res = dynamic_cast<ResourceEntity*>(worker->getThingToInteract());
		if (res && res->getResourceId() == worstResId) { return worker; }
	}
	return nullptr;
}

std::vector<Unit*> AiOrchestrator::findFreeWorkers() const {
	std::vector<Unit*> freeWorkers;
	freeWorkers.reserve(possession->getWorkers().size());
	std::ranges::copy_if(possession->getWorkers(),
	                     std::back_inserter(freeWorkers),
	                     isFreeWorker);
	return freeWorkers;
}

Physical* AiOrchestrator::closestInRange(Unit* worker, int resourceId) {
	const auto env = Game::getEnvironment();
	const int levelCount = env->getResourceLevelCount();
	for (int level = 0; level < levelCount; ++level) {
		const auto& list = env->getResources(worker->getPosition(), resourceId, level);
		const auto closest = env->closestPhysical(worker->getMainGridIndex(), list, belowClose, false);
		if (closest) { return closest; }
	}
	return nullptr;
}
