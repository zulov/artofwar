#include "AiOrchestrator.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <optional>
#include <ranges>
#include <utility>
#include <valarray>
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

namespace {
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
	};

	constexpr ArmyTargetSpec ARMY_TARGET_SPECS[] = {
			{MilitaryCenterIdx::OUR_ARMY, CenterType::ARMY, false, AiOrderType::DEFEND_ARMY},
			{MilitaryCenterIdx::OUR_ECON, CenterType::ECON, false, AiOrderType::DEFEND_ECON},
			{MilitaryCenterIdx::OUR_BUILDING, CenterType::BUILDING, false, AiOrderType::DEFEND_BUILDING},
			{MilitaryCenterIdx::ENEMY_ARMY, CenterType::ARMY, true, AiOrderType::ATTACK_ARMY},
			{MilitaryCenterIdx::ENEMY_ECON, CenterType::ECON, true, AiOrderType::ATTACK_ECON},
			{MilitaryCenterIdx::ENEMY_BUILDING, CenterType::BUILDING, true, AiOrderType::ATTACK_BUILDING},
			{MilitaryCenterIdx::BATTLE, CenterType::BATTLE, false, AiOrderType::MOVE_BATTLE},
	};
	constexpr size_t ARMY_TARGET_SPEC_COUNT = sizeof(ARMY_TARGET_SPECS) / sizeof(ARMY_TARGET_SPECS[0]);

	constexpr size_t armyTargetIndex(MilitaryCenterIdx center) {
		for (size_t i = 0; i < ARMY_TARGET_SPEC_COUNT; ++i) { if (ARMY_TARGET_SPECS[i].center == center) { return i; } }
		return ARMY_TARGET_SPEC_COUNT;
	}

	template <typename Candidate, typename ScoreFn>
	std::vector<float> scoreCandidates(const std::vector<Candidate*>& candidates, const ScoreFn& scoreFn) {
		std::vector<float> scores;
		scores.reserve(candidates.size());
		for (auto* candidate : candidates) { scores.push_back(scoreFn(candidate)); }
		return scores;
	}
}


AiOrchestrator::AiOrchestrator(Player* player, db_nation* nation, AiHistory* history) :
	player(player), playerId(player->getId()), possession(player->getPossession()), nation(nation),
	history(history),
	masterBrain(nation),
	economyBrain(nation),
	unitBrain(nation),
	militaryBrain(nation),
	militaryCommandCalculator(MILITARY_COMMAND_RADIUS),
	wantExecutor(player, nation, history) { lastLacking.reset(); }

void AiOrchestrator::createWorkers() {
	const short workerId = resolveWorkerId();
	if (lastEconOut.workerCount > 0 && workerId >= 0) {
		tryUnitWant(WantItemType::WORKER, lastEconOut.workerAllocation, workerId, lastEconOut.workerCount);
	}
}

void AiOrchestrator::createUnits(const UnitOutput& unitOut, std::span<const float> unitProfileDiffs) {
	if (unitOut.count > 0) {
		for (auto* unit : resolveUnit(unitOut, unitProfileDiffs)) {
			tryUnitWant(WantItemType::UNIT, lastMasterOut.unitUrgency, unit->id, unitOut.count);
		}
	}
}

void AiOrchestrator::upgradeUnits(const UnitOutput& unitOut, std::span<const float> unitProfileDiffs) {
	if (unitOut.unitUpgradeUrgency > 0.1f) {
		if (auto* unitToUpgrade = resolveUnitUpgrade(unitProfileDiffs)) {
			tryUnitWant(WantItemType::UNIT_UPGRADE, unitOut.unitUpgradeUrgency, unitToUpgrade->id);
		}
	}
}

void AiOrchestrator::upgradeWorkers() {
	if (lastEconOut.workerUpgradeUrgency > 0.1f) {
		if (auto* found = resolveWorkerUpgrade()) {
			tryUnitWant(WantItemType::UNIT_UPGRADE, lastEconOut.workerUpgradeUrgency, found->id);
		}
	}
}

void AiOrchestrator::upgradeUnitBuilding(const UnitOutput& unitOut, std::span<const float> unitProfileDiffs) {
	if (unitOut.buildingUpgradeUrgency > 0.1f) {
		if (auto* found = resolveBuildingUpgrade(unitProfileDiffs)) {
			tryToUpgradeBuilding(found->id, unitOut.buildingUpgradeUrgency);
		}
	}
}

void AiOrchestrator::upgradeResBuilding() {
	if (lastEconOut.resBuildingUpgradeUrgency > 0.1f) {
		if (auto* found = resolveResBuildingUpgrade(lastEconOut)) {
			tryToUpgradeBuilding(found->id, lastEconOut.resBuildingUpgradeUrgency);
		}
	}
}

void AiOrchestrator::tryToUpgradeBuilding(unsigned short id, float priority) {
	if (hasOwnedBuildingInstance(id)) {
		if (player->getNextBuildingLevel(id).has_value()) {
			wantList.addRequest(WantItemType::BUILDING_UPGRADE, priority, id);
		}
	} else {
		wantList.addRequest(WantItemType::BUILDING, priority, id);
	}
}

//TODO AI name functions that determine resource type eg. isResBonus isFoodStarage and use it where posible put it in db_building and db_building_level
//TODO AI better scoring if in more than ona category how to sum it
void AiOrchestrator::createResBuilding() {
	struct Candidate {
		float need;
		unsigned short id;
	};
	std::vector<Candidate> candidates;

	for (const auto b : getPossibleBuildingsInType(ParentBuildingType::RESOURCE)) {
		auto* l = player->getBuildingLevel(b->id);
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
		if (need > 0.1f) { candidates.push_back({.need = need, .id = b->id}); }
	}

	const size_t keep = std::min<size_t>(candidates.size(), MAX_RES_BUILDING_REQUESTS);
	std::ranges::partial_sort(candidates, candidates.begin() + keep,
	                          [](const Candidate& a, const Candidate& b) { return a.need > b.need; });
	for (size_t i = 0; i < keep; ++i) {
		wantList.addRequest(WantItemType::BUILDING, candidates[i].need, candidates[i].id);
	}
}

// Missing-producer fallback: queue the building once, then retry the original want next tick.
void AiOrchestrator::tryUnitWant(WantItemType type, float priority, unsigned short unitId, unsigned char count) {
	// One hop only: if the desired thing cannot run because its producer is missing,
	// request that producer building and let the AI re-issue the original want next tick.
	// TODO: if the producer exists but still needs an upgrade, route that upgrade too.
	assert(type != WantItemType::BUILDING);

	if (hasAnyBuildingThatDeploy(unitId)) {
		wantList.addRequest(type, priority, unitId, count);
		return;
	}
	const auto buildingId = findBuildingToBuild(unitId);
	if (buildingId >= 0) {
		wantList.addRequest(WantItemType::BUILDING, priority, buildingId);
	}
}

bool AiOrchestrator::hasOwnedBuildingInstance(unsigned short buildingId) const {
	return !possession->getBuildings(buildingId)->empty();
}

void AiOrchestrator::action() {
	const auto enemy = Game::getPlayersMan()->getEnemyFor(playerId);

	lastMasterOut = masterBrain.decide(player, enemy, lastLacking.totalSum, history);

	float gameTime = norm(Game::getFrameInfo()->getSeconds(), NormScale::GAME_TIME_SHORT);
	lastEconOut = economyBrain.decide(player, enemy,
			lastLacking.perResource,
			lastMasterOut.economyUrgency, lastMasterOut.workerUrgency, lastMasterOut.expandUrgency,
			lastMasterOut.techUrgency, gameTime, history);

	// 3. Military Brain (composition prefs feed into UnitBrain)
	float techLevel = avgTechLevel(nation->units, nation->buildings, player);
	lastMilOut = militaryBrain.decide(player, enemy,
			lastMasterOut.militaryUrgency, lastMasterOut.attackUrgency,
			techLevel, history);

	// 4. Unit Brain
	auto unitOut = unitBrain.decide(player, enemy,
			lastMasterOut.unitUrgency, lastMasterOut.attackUrgency,
			lastMilOut.preferInfantry, lastMilOut.preferRange, lastMilOut.preferCavalry,
			lastMasterOut.techUrgency, gameTime);
	const auto unitProfileDiffs = calculateUnitProfileDiffs(unitOut.unitProfile);

	// 5. Submit requests to WantList
	wantList.resetRequests();

	createWorkers();
	upgradeWorkers();

	createUnits(unitOut, unitProfileDiffs);
	upgradeUnits(unitOut, unitProfileDiffs);

	// Unit-producing building upgrade request (barracks, archery range, stable)
	upgradeUnitBuilding(unitOut, unitProfileDiffs);

	// Resource building upgrade request (farms, mills, mines, refineries, etc.)
	upgradeResBuilding();

	// Defence building upgrade request (tower)
	//submitBuildingUpgradeRequest(lastMasterOut.defenceBuildingUrgency, ParentBuildingType::DEFENCE);
	// Other building upgrade request (center, house)
	//submitBuildingUpgradeRequest(lastMasterOut.buildingUrgency, ParentBuildingType::OTHER);
	// Tech building upgrade request (blacksmith, university)
	//submitBuildingUpgradeRequest(lastMasterOut.techUrgency, ParentBuildingType::TECH);

	// Building requests — use MasterBrain urgencies directly
	// submitBuildingRequest(lastMasterOut.defenceBuildingUrgency, ParentBuildingType::DEFENCE);
	// submitBuildingRequest(lastMasterOut.buildingUrgency, ParentBuildingType::OTHER);
	// submitBuildingRequest(lastMasterOut.techUrgency, ParentBuildingType::TECH);

	createResBuilding();

	// 6. Execute WantList
	wantExecutor.prepare(lastMasterOut);
	lastLacking = wantList.execute(player->getResources()->getValues(), wantExecutor);
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
			&& player->getNextBuildingLevel(building->id).has_value()) {
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

	// Per-unit decision: each unit picks the highest-pressure target at its own
	// position. Units that fall below the pressure threshold (or pick a center
	// whose snapshot is unavailable) are routed to hold.
	std::array<std::vector<std::pair<Unit*, float>>, ARMY_TARGET_SPEC_COUNT> buckets{};
	std::vector<std::pair<Unit*, MilitaryCenterIdx>> holdUnits;
	for (auto* unit : allArmy) {
		if (unit->getCommandPriority() >= MAX_COMMAND_PRIORITY) { continue; }
		const auto unitResult = militaryCommandCalculator.calculate(unit->getPosition(), centers, lastMilOut);
		const auto specIndex = armyTargetIndex(unitResult.best.center);
		if (unitResult.best.score <= MIN_ARMY_ORDER_PRESSURE || specIndex == ARMY_TARGET_SPEC_COUNT) {
			holdUnits.push_back({unit, unitResult.best.center});
			continue;
		}
		buckets[specIndex].push_back({unit, unitResult.best.score});
	}

	// Each bucket goes directly to the center selected by MilitaryBrain pressure.
	for (size_t i = 0; i < ARMY_TARGET_SPEC_COUNT; ++i) {
		if (buckets[i].empty()) { continue; }
		const auto& spec = ARMY_TARGET_SPECS[i];
		const auto& bestTarget = centers[castC(spec.center)];
		if (!bestTarget.has_value()) {
			for (const auto& entry : buckets[i]) { holdUnits.push_back({entry.first, spec.center}); }
			continue;
		}
		std::ranges::sort(buckets[i], [&](const auto& a, const auto& b) {
			return a.first->getPosition().SqDistXZ(*bestTarget) < b.first->getPosition().SqDistXZ(*bestTarget);
		});
		issueAdvancePerUnit(buckets[i], spec.center, *bestTarget);
		history->addOrder(spec.orderType, AiOrderResult::SUCCESS, static_cast<uint8_t>(buckets[i].size()));
	}

	if (!holdUnits.empty()) {
		history->addOrder(AiOrderType::NONE, AiOrderResult::NO_CENTER_POSITION);
		issueHold(holdUnits, MIN_ARMY_ORDER_PRESSURE);
	}
}

void AiOrchestrator::decayUnitOrderPriorities() const {
	for (auto* unit : possession->getAllArmy()) { unit->decayCommandPriority(COMMAND_PRIORITY_DECAY_MULTIPLIER); }
}

bool AiOrchestrator::trySubmitUnitOrder(const std::vector<Unit*>& units, float priority, MilitaryCenterIdx center,
                                        UnitOrder* order) const {
	// TODO: Check later whether this remains necessary with caller-side priority checks.
	for (auto* unit : units) {
		if (priority <= unit->getCommandPriority()) {
			delete order;
			return false;
		}
	}

	for (auto* unit : units) {
		unit->setCommandPriority(priority * COMMAND_PRIORITY_MULTIPLIER);
		unit->setCommandCenter(center);
	}
	Game::getActionCenter()->addUnitAction(order);
	return true;
}

// Advance toward target per unit. A matching center refreshes an active order;
// a different center must exceed the remaining lock to replace it.
void AiOrchestrator::issueAdvancePerUnit(const std::vector<std::pair<Unit*, float>>& units,
                                         MilitaryCenterIdx center, const Urho3D::Vector2& target) {
	for (const auto& [unit, priority] : units) {
		if (unit->hasAim() && unit->getCommandPriority() > 0.f && unit->getCommandCenter() == center) {
			unit->setCommandPriority(priority * COMMAND_PRIORITY_MULTIPLIER);
			continue;
		}
		if (priority <= unit->getCommandPriority()) { continue; }
		if (unit->getPosition().SqDistXZ(target) > SQ_SEMI_CLOSE) {
			trySubmitUnitOrder(std::vector<Unit*>{unit}, priority, center,
			                   new IndividualOrder(unit, UnitAction::GO, target));
		} else
			if (tryIssueNearbyAttack(unit, priority, center)) { continue; }
	}
}

// Hold — only free units engage nearby enemies
void AiOrchestrator::issueHold(std::vector<std::pair<Unit*, MilitaryCenterIdx>>& group, float priority) {
	for (const auto& [unit, center] : group) {
		if (priority <= unit->getCommandPriority()) { continue; }
		if (!isFree(unit)) { continue; }
		tryIssueNearbyAttack(unit, priority, center);
	}
}

bool AiOrchestrator::tryIssueNearbyAttack(Unit* unit, float priority, MilitaryCenterIdx center) const {
	auto& things = Game::getEnvironment()->getNeighboursFromTeamNotEq(unit, SEMI_CLOSE);
	if (things.empty()) { return false; }
	const auto closest = Game::getEnvironment()->
			closestPhysical(unit->getMainGridIndex(), things, belowClose, true);
	if (!closest) { return false; }
	return trySubmitUnitOrder(std::vector<Unit*>{unit}, priority, center,
	                          new IndividualOrder(unit, UnitAction::ATTACK, closest));
}

// --- Unit resolution ---
// TODO: Consider producing more samples than 'count' (e.g. count + N extras) and storing
// the extras as fallback candidates in WantItem. When executeUnit() fails for the primary
// unit (no building), it could try the fallback before giving up. This avoids wasting a
// tick when the ideal unit is unbuildable but a similar one is available.

std::vector<float> AiOrchestrator::calculateUnitProfileDiffs(std::span<const float> unitProfile) const {
	std::valarray center(unitProfile.data(), unitProfile.size());
	std::vector<float> diffs(Game::getDatabase()->getUnits().size(), std::numeric_limits<float>::max());

	for (auto* unit : Game::getDatabase()->getUnits()) {
		if (unit) {
			diffs[unit->id] = sumSquaredError(center, player->getUnitLevel(unit->id)->dbUnitMetric->getValuesNormAsVal());
		}
	}
	return diffs;
}

std::vector<db_unit*> AiOrchestrator::resolveUnit(const UnitOutput& unitOutput,
                                                  std::span<const float> unitProfileDiffs) {
	auto& units = nation->units;
	std::vector<db_unit*> candidates;
	candidates.reserve(units.size());
	for (auto unit : units) { if (!unit->typeWorker) { candidates.push_back(unit); } }
	if (candidates.empty()) { return {}; }

	const auto diffs = scoreCandidates(candidates, [&unitProfileDiffs](db_unit* unit) {
		return unitProfileDiffs[unit->id];
	});

	std::vector<db_unit*> result;
	result.reserve(unitOutput.count);
	for (const auto inx : lowestWithRand(diffs, unitOutput.count)) { result.push_back(candidates[inx]); }
	return result;
}

db_unit* AiOrchestrator::resolveUnitUpgrade(std::span<const float> unitProfileDiffs) {
	auto& units = nation->units;
	std::vector<db_unit*> candidates;
	candidates.reserve(units.size());
	for (auto unit : units) {
		if (!unit->typeWorker && player->getNextUnitLevel(unit->id).has_value()) { candidates.push_back(unit); }
	}
	if (candidates.empty()) { return nullptr; }

	const auto diffs = scoreCandidates(candidates, [&unitProfileDiffs](db_unit* unit) {
		return unitProfileDiffs[unit->id];
	});

	return candidates[lowestWithRand(diffs)];
}

db_building* AiOrchestrator::resolveBuildingUpgrade(std::span<const float> unitProfileDiffs) {
	auto& buildings = nation->buildings;
	std::vector<db_building*> candidates;
	candidates.reserve(buildings.size());
	for (auto building : buildings) {
		if (building->parentType[static_cast<int>(ParentBuildingType::UNITS)]
			&& player->getNextBuildingLevel(building->id).has_value()) { candidates.push_back(building); }
	}
	if (candidates.empty()) { return nullptr; }

	// Match building by which units it produces — find the building whose units
	// are closest to the desired unit profile
	auto diffs = scoreCandidates(candidates, [&unitProfileDiffs, this](db_building* building) {
		// Average distance across all units this building can produce
		auto* level = player->getBuildingLevel(building->id);
		float totalDist = 0.f;
		int unitCount = 0;
		for (auto* unit : level->allUnits) {
			if (!unit->typeWorker) {
				totalDist += unitProfileDiffs[unit->id];
				++unitCount;
			}
		}
		return unitCount > 0 ? totalDist / static_cast<float>(unitCount) : std::numeric_limits<float>::max();
	});

	return candidates[lowestWithRand(diffs)];
}

db_unit* AiOrchestrator::resolveWorkerUpgrade() {
	for (auto* worker : nation->workers) { if (player->getNextUnitLevel(worker->id).has_value()) { return worker; } }
	return nullptr;
}

// TODO: pick a worker type intentionally; for now just use the first one the nation has.
short AiOrchestrator::resolveWorkerId() const { return nation->workers.empty() ? -1 : nation->workers.at(0)->id; }

db_building* AiOrchestrator::resolveResBuildingUpgrade(const EconomyOutput& econOutput) const {
	auto& buildings = nation->buildings;
	std::vector<db_building*> candidates;
	candidates.reserve(buildings.size());
	for (auto* building : buildings) {
		if (building->parentType[static_cast<int>(ParentBuildingType::RESOURCE)]
			&& player->getNextBuildingLevel(building->id).has_value()) { candidates.push_back(building); }
	}
	if (candidates.empty()) { return nullptr; }
	if (candidates.size() == 1) { return candidates[0]; }

	// Weight by resource type priority + subtype need signals
	auto weights = scoreCandidates(candidates, [this, &econOutput](db_building* building) {
		float weight = 0.1f; // base weight
		auto* level = player->getBuildingLevel(building->id);

		// Resource type priority
		if (building->typeResourceFood) {
			weight += std::max(0.f, econOutput.foodPriority);
		}
		if (building->typeResourceWood) {
			weight += std::max(0.f, econOutput.woodPriority);
		}
		if (building->typeResourceStone) {
			weight += std::max(0.f, econOutput.stonePriority);
		}
		if (building->typeResourceGold) {
			weight += std::max(0.f, econOutput.goldPriority);
		}

		// Subtype need signals — distinguish between buildings of the same resource type
		if (level->foodStorage > 0) {
			weight += std::max(0.f, econOutput.needFoodStorage);
		}
		if (level->goldStorage > 0) {
			weight += std::max(0.f, econOutput.needGoldStorage);
		}
		if (level->stoneRefineCapacity > 0.f) {
			weight += std::max(0.f, econOutput.needStoneRefine);
		}
		if (level->goldRefineCapacity > 0.f) {
			weight += std::max(0.f, econOutput.needGoldRefine);
		}
		if (level->collect > 0.f && level->resourceRange > 0.f) {
			if (building->typeResourceFood) {
				weight += std::max(0.f, econOutput.needBonusFood);
			}
			if (building->typeResourceWood) {
				weight += std::max(0.f, econOutput.needBonusWood);
			}
			if (building->typeResourceStone) {
				weight += std::max(0.f, econOutput.needBonusStone);
			}
			if (building->typeResourceGold) {
				weight += std::max(0.f, econOutput.needBonusGold);
			}
		}
		if (building->toResource >= 0 && level->spawnResourceRange <= 0) {
			weight += std::max(0.f, econOutput.needFoodSource);
		}
		if (building->toResource >= 0 && level->spawnResourceRange > 0) {
			weight += std::max(0.f, econOutput.needWoodSource);
		}

		return weight;
	});

	float totalWeight = 0.f;
	for (float w : weights) { totalWeight += w; }
	return candidates[sampleWeighted(weights, totalWeight)];
}
short AiOrchestrator::findBuildingToBuild(unsigned short unitId) const {
	//std::vector <canditeds>//TODO potencjalnie moze byc wiecej niz jeden
	for (const auto building : nation->buildings) {
		if (building->canEverProduceUnit(player->getNation(), unitId)) {
			return building->id;
		}
	}
	return -1;
}

bool AiOrchestrator::hasAnyBuildingThatDeploy(unsigned short unitId) const {
	for (const auto building : possession->getBuildings()) {
		if (building->getDb()->canEverProduceUnit(player->getNation(), unitId)) {
			return true;
		}
	}
	return false;
}

// --- Building resolution ---

db_building* AiOrchestrator::resolveBuilding(ParentBuildingType type) {
	const auto buildings = getPossibleBuildingsInType(type);
	if (buildings.empty()) { return nullptr; }
	if (buildings.size() == 1) { return buildings.at(0); }

	// TODO: use building metric matching with brain output
	return buildings.at(0);
}

std::vector<db_building*> AiOrchestrator::getPossibleBuildingsInType(ParentBuildingType type) const {
	std::vector<db_building*> buildings;
	buildings.reserve(nation->buildings.size());
	for (auto dbBuilding : nation->buildings) {
		if (dbBuilding->parentType[castC(type)]) { buildings.push_back(dbBuilding); }
	}
	return buildings;
}

// --- Worker collection ---

void AiOrchestrator::manageWorkers() {
	auto freeWorkers = findFreeWorkers();

	// Use economy brain's resource priorities to decide which resource to collect
	float prefs[] = {lastEconOut.foodPriority, lastEconOut.woodPriority,
	                 lastEconOut.stonePriority, lastEconOut.goldPriority};
	std::array order = {0, 1, 2, 3};
	std::ranges::sort(order, [&](int a, int b) { return prefs[a] > prefs[b]; });

	// Reassign one busy worker away from the most negative resource.
	if (auto* worker = findReassignableWorker(order, prefs)) { freeWorkers.push_back(worker); }

	if (freeWorkers.empty()) { return; }

	// Split workers across resources proportionally to their needs.
	std::array<int, RESOURCES_SIZE> remaining = computeWorkerTargets(prefs, freeWorkers.size());

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
