#include "AiOrchestrator.h"

#include <cmath>
#include <limits>
#include "AiHistory.h"
#include "AiUtils.h"
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
#include "objects/unit/order/UnitConst.h"
#include "objects/unit/GroupUtils.h"
#include "objects/unit/order/GroupOrder.h"
#include "objects/PhysicalUtils.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/state/UnitState.h"
#include "env/influence/CenterType.h"
#include "database/DatabaseCache.h"

namespace {
	bool isResBonus(db_building* b, db_building_level* l, ResourceType res) {
		return b->resourceType == cast(res) && l->collect > 0.f && l->resourceRange > 0.f;
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
	wantExecutor(player, nation, history) { lastLacking.reset(); }

void AiOrchestrator::action() {
	const auto enemy = Game::getPlayersMan()->getEnemyFor(playerId);

	// 1. Master Brain decides urgencies
	lastMasterOut = masterBrain.decide(
			player, enemy,
			lastLacking.totalSum,
			history
			);

	// 2. Economy Brain (gets lacking feedback)
	float gameTime = norm(Game::getFrameInfo()->getSeconds(), 600.f); // normalize to ~10 min
	lastEconOut = economyBrain.decide(
			player, enemy,
			lastLacking.perResource,
			lastMasterOut.economyUrgency, lastMasterOut.workerUrgency, lastMasterOut.expandUrgency,
			lastMasterOut.techUrgency, gameTime,
			history
			);

	// 3. Military Brain (composition prefs feed into UnitBrain)
	lastMilOut = militaryBrain.decide(
			player, enemy,
			lastMasterOut.militaryUrgency, lastMasterOut.attackUrgency,
			history
			);

	// 4. Unit Brain
	auto unitOut = unitBrain.decide(
			player, enemy,
			lastMasterOut.unitUrgency, lastMasterOut.attackUrgency,
			lastMilOut.preferInfantry, lastMilOut.preferRange, lastMilOut.preferCavalry,
			lastMasterOut.techUrgency, gameTime
			);

	// 4. Submit requests to WantList
	wantList.resetRequests();

	// Worker request
	if (lastEconOut.workerCount > 0) {
		wantList.addRequest(WantItemType::WORKER, lastEconOut.workerAllocation, workerId, lastEconOut.workerCount);
	}

	// Unit request
	if (unitOut.count > 0) {
		for (auto* unit : resolveUnit(unitOut)) {
			wantList.addRequest(WantItemType::UNIT, lastMasterOut.unitUrgency, unit->id);
		}
	}

	// Unit upgrade request
	if (unitOut.unitUpgradeUrgency > 0.1f) {
		if (auto* unitToUpgrade = resolveUnitUpgrade(unitOut)) {
			wantList.addRequest(WantItemType::UNIT_UPGRADE, unitOut.unitUpgradeUrgency, unitToUpgrade->id);
		}
	}

	// Unit-producing building upgrade request (barracks, archery range, stable)
	if (unitOut.buildingUpgradeUrgency > 0.1f) {
		if (auto* buildingToUpgrade = resolveBuildingUpgrade(unitOut)) {
			wantList.addRequest(WantItemType::BUILDING_UPGRADE, unitOut.buildingUpgradeUrgency, buildingToUpgrade->id);
		}
	}

	// Worker upgrade request
	if (lastEconOut.workerUpgradeUrgency > 0.1f) {
		if (auto* workerToUpgrade = resolveWorkerUpgrade()) {
			wantList.addRequest(WantItemType::UNIT_UPGRADE, lastEconOut.workerUpgradeUrgency, workerToUpgrade->id);
		}
	}

	// Resource building upgrade request (farms, mills, mines, refineries, etc.)
	if (lastEconOut.resBuildingUpgradeUrgency > 0.1f) {
		if (auto* resBuildingToUpgrade = resolveResBuildingUpgrade(lastEconOut)) {
			wantList.addRequest(WantItemType::BUILDING_UPGRADE, lastEconOut.resBuildingUpgradeUrgency,
			                    resBuildingToUpgrade->id);
		}
	}

	// Defence building upgrade request (tower)
	submitBuildingUpgradeRequest(lastMasterOut.defenceBuildingUrgency, ParentBuildingType::DEFENCE);
	// Other building upgrade request (center, house)
	submitBuildingUpgradeRequest(lastMasterOut.buildingUrgency, ParentBuildingType::OTHER);
	// Tech building upgrade request (blacksmith, university)
	submitBuildingUpgradeRequest(lastMasterOut.techUrgency, ParentBuildingType::TECH);

	// Building requests — use MasterBrain urgencies directly
	submitBuildingRequest(lastMasterOut.buildingUrgency, ParentBuildingType::OTHER);
	submitBuildingRequest(lastMasterOut.defenceBuildingUrgency, ParentBuildingType::DEFENCE);
	submitBuildingRequest(lastMasterOut.techUrgency, ParentBuildingType::TECH);

	// Resource buildings — submit each need individually
	const auto buildings = getBuildingsInType(ParentBuildingType::RESOURCE);

	findAndSubmit(lastEconOut.needBonusFood, buildings,
	              [](auto* b, auto* l) { return isResBonus(b, l, ResourceType::FOOD); });
	findAndSubmit(lastEconOut.needBonusWood, buildings,
	              [](auto* b, auto* l) { return isResBonus(b, l, ResourceType::WOOD); });
	findAndSubmit(lastEconOut.needBonusStone, buildings,
	              [](auto* b, auto* l) { return isResBonus(b, l, ResourceType::STONE); });
	findAndSubmit(lastEconOut.needBonusGold, buildings,
	              [](auto* b, auto* l) { return isResBonus(b, l, ResourceType::GOLD); });
	findAndSubmit(lastEconOut.needFoodSource, buildings, [](auto* b, auto* l) {
		return b->toResource >= 0 && l->spawnResourceRange <= 0;
	});
	findAndSubmit(lastEconOut.needWoodSource, buildings, [](auto* b, auto* l) {
		return b->toResource >= 0 && l->spawnResourceRange > 0;
	});
	findAndSubmit(lastEconOut.needFoodStorage, buildings, [](auto* b, auto* l) { return l->foodStorage > 0; });
	findAndSubmit(lastEconOut.needGoldStorage, buildings, [](auto* b, auto* l) { return l->goldStorage > 0; });
	findAndSubmit(lastEconOut.needStoneRefine, buildings, [](auto* b, auto* l) { return l->stoneRefineCapacity > 0.f; });
	findAndSubmit(lastEconOut.needGoldRefine, buildings, [](auto* b, auto* l) { return l->goldRefineCapacity > 0.f; });

	// UNITS building: prefer specific building from lacking feedback, fall back to generic resolve
	if (lastLacking.lackingBuildingForUnit >= 0) {
		wantList.addRequest(WantItemType::BUILDING, std::max(lastMasterOut.unitUrgency, 0.5f),
		                    lastLacking.lackingBuildingForUnit);
	} else { submitBuildingRequest(lastMasterOut.unitUrgency, ParentBuildingType::UNITS); }

	// 5. Execute WantList
	wantExecutor.prepare(lastMasterOut);
	lastLacking = wantList.execute(player, wantExecutor);
	lastLacking.lackingBuildingForUnit = wantExecutor.getLackingBuilding();
}

void AiOrchestrator::submitBuildingRequest(float urgency, ParentBuildingType type) {
	if (urgency > 0.1f) {
		if (db_building* building = resolveBuilding(type)) {
			wantList.addRequest(WantItemType::BUILDING, urgency, building->id);
		}
	}
}

template <typename Match>
void AiOrchestrator::findAndSubmit(float need, const std::vector<db_building*>& buildings, Match&& match) {
	if (need <= 0.1f) { return; }
	for (const auto b : buildings) {
		auto* level = player->getLevelForBuilding(b->id);
		if (match(b, level)) {
			wantList.addRequest(WantItemType::BUILDING, need, b->id);
			return;
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
	// Worker collection
	menageWorkers();

	const auto enemy = Game::getPlayersMan()->getEnemyFor(playerId);

	// Get ALL army (free + busy) for stance-based control
	std::vector<Unit*> allArmy = possession->getAllArmy();
	if (allArmy.empty()) { return; }

	const auto totalArmy = allArmy.size();

	int attackCount = roundToInt(lastMilOut.attackRatio * totalArmy);
	int defendCount = roundToInt(lastMilOut.defendRatio * totalArmy);

	// Clamp so attack + defend <= total
	if (attackCount + defendCount > totalArmy) {
		float sum = lastMilOut.attackRatio + lastMilOut.defendRatio;
		if (sum > 0.f) {
			attackCount = roundToInt((lastMilOut.attackRatio / sum) * totalArmy);
			defendCount = totalArmy - attackCount;
		} else {
			attackCount = 0;
			defendCount = 0;
		}
	}

	// Resolve target positions
	auto spatialOut = attackSpatialBrain.decide(
			player, enemy, lastMilOut,
			lastMasterOut.militaryUrgency, lastMasterOut.attackUrgency
			);

	auto attackPosOpt = [&]() -> std::optional<Urho3D::Vector2> {
		auto areas = Game::getEnvironment()->getAreas(
				playerId, std::span<const float>(spatialOut.weights), 1);
		if (!areas.empty()) { return areas[0]; }
		return Game::getEnvironment()->getCenterOf(CenterType::BUILDING, enemy->getId());
	}();

	auto defendPosOpt = Game::getEnvironment()->getCenterOf(CenterType::BUILDING, playerId);

	if (!attackPosOpt.has_value() && !defendPosOpt.has_value()) {
		history->addOrder(AiOrderType::NONE, AiOrderResult::NO_CENTER_POSITION);
		return;
	}

	// Distance-based assignment over ALL army
	if (attackPosOpt.has_value() && attackCount > 0) {
		std::sort(allArmy.begin(), allArmy.end(), [&](const Unit* a, const Unit* b) {
			return a->getPosition().SqDistXZ(attackPosOpt.value()) < b->getPosition().SqDistXZ(attackPosOpt.value());
		});
	}

	std::vector<Unit*> attackGroup(allArmy.begin(), allArmy.begin() + std::min(attackCount, totalArmy));
	std::vector<Unit*> remaining(allArmy.begin() + static_cast<int>(attackGroup.size()), allArmy.end());

	if (defendPosOpt.has_value() && defendCount > 0 && !remaining.empty()) {
		std::sort(remaining.begin(), remaining.end(), [&](const Unit* a, const Unit* b) {
			return a->getPosition().SqDistXZ(defendPosOpt.value()) < b->getPosition().SqDistXZ(defendPosOpt.value());
		});
	}

	std::vector<Unit*> defendGroup(remaining.begin(),
	                               remaining.begin() + std::min(defendCount, static_cast<int>(remaining.size())));

	constexpr float SEMI_CLOSE = 30.f;
	constexpr float SQ_SEMI_CLOSE = SEMI_CLOSE * SEMI_CLOSE;
	constexpr float STANCE_ADVANCE = 0.3f;
	constexpr float STANCE_RETREAT = -0.3f;

	// Helper: advance group toward target, attack when close
	auto issueAdvance = [&](std::vector<Unit*>& group, const Urho3D::Vector2& target) {
		for (auto& subArmy : divide(group)) {
			auto armyCenter = computeCenter(subArmy);
			if (armyCenter.SqDistXZ(target) > SQ_SEMI_CLOSE) {
				Game::getActionCenter()->addUnitAction(
						subArmy.size() > 1
						? static_cast<UnitOrder*>(new GroupOrder(subArmy, UnitActionType::ORDER, castC(UnitAction::GO),
						                                         target))
						: static_cast<UnitOrder*>(new IndividualOrder(subArmy.at(0), UnitAction::GO, target))
						);
			} else {
				const auto unit = subArmy.at(0);
				auto* things = Game::getEnvironment()->getNeighboursFromTeamNotEq(unit, SEMI_CLOSE);
				if (things && !things->empty()) {
					const auto closest = Game::getEnvironment()->closestPhysical(
							unit->getMainGridIndex(), things, belowClose, true);
					if (closest) {
						Game::getActionCenter()->addUnitAction(
								new GroupOrder(subArmy, UnitActionType::ORDER,
								               castC(UnitAction::ATTACK), closest));
					}
				}
			}
		}
	};

	// Helper: hold — only free units engage nearby enemies
	auto issueHold = [&](std::vector<Unit*>& group) {
		for (auto* unit : group) {
			if (!isFree(unit)) { continue; }
			auto* things = Game::getEnvironment()->getNeighboursFromTeamNotEq(unit, SEMI_CLOSE);
			if (things && !things->empty()) {
				const auto closest = Game::getEnvironment()->closestPhysical(
						unit->getMainGridIndex(), things, belowClose, true);
				if (closest) {
					Game::getActionCenter()->addUnitAction(
							new IndividualOrder(unit, UnitAction::ATTACK, closest));
				}
			}
		}
	};

	// Issue orders for attack group
	if (!attackGroup.empty() && attackPosOpt.has_value()) {
		if (lastMilOut.attackStance < STANCE_RETREAT && defendPosOpt.has_value()) {
			// Retreat: pull back toward own base
			for (auto* unit : attackGroup) {
				Game::getActionCenter()->addUnitAction(
						new IndividualOrder(unit, UnitAction::GO, defendPosOpt.value())
						);
			}
		} else if (lastMilOut.attackStance < STANCE_ADVANCE) { issueHold(attackGroup); } else {
			issueAdvance(attackGroup, attackPosOpt.value());
		}
		history->addOrder(AiOrderType::ATTACK_ECON, AiOrderResult::SUCCESS, static_cast<uint8_t>(attackGroup.size()));
	}

	// Issue orders for defend group
	if (!defendGroup.empty() && defendPosOpt.has_value()) {
		if (lastMilOut.defendStance < STANCE_RETREAT) {
			// Disengage: stop all units, let them idle
			for (auto* unit : defendGroup) {
				Game::getActionCenter()->addUnitAction(
						new IndividualOrder(unit, UnitAction::STOP, unit->getPosition())
						);
			}
		} else if (lastMilOut.defendStance < STANCE_ADVANCE) { issueHold(defendGroup); } else {
			issueAdvance(defendGroup, defendPosOpt.value());
		}
		history->addOrder(AiOrderType::DEFEND_ECON, AiOrderResult::SUCCESS, static_cast<uint8_t>(defendGroup.size()));
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

	std::valarray<float> center(unitOutput.unitProfile.data(), unitOutput.unitProfile.size());
	std::vector<float> diffs;
	diffs.reserve(candidates.size());
	for (const auto unit : candidates) {
		diffs.push_back(dist(center, player->getLevelForUnit(unit->id)->dbUnitMetric));
	}

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

	std::valarray<float> center(unitOutput.unitProfile.data(), unitOutput.unitProfile.size());
	std::vector<float> diffs;
	diffs.reserve(candidates.size());
	for (const auto unit : candidates) {
		diffs.push_back(dist(center, player->getLevelForUnit(unit->id)->dbUnitMetric));
	}

	return candidates[lowestWithRand(diffs)];
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

db_building* AiOrchestrator::resolveResBuildingUpgrade(const EconomyOutput& econOutput) {
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

void AiOrchestrator::menageWorkers() {
	auto freeWorkers = findFreeWorkers();

	// Use economy brain's resource priorities to decide which resource to collect
	float prefs[] = {lastEconOut.foodPriority, lastEconOut.woodPriority,
	                 lastEconOut.stonePriority, lastEconOut.goldPriority};
	std::array<int, 4> order = {0, 1, 2, 3};
	std::ranges::sort(order, [&](int a, int b) { return prefs[a] > prefs[b]; });

	// Reassign one busy worker from lowest-priority resource to highest
	if (lastEconOut.reassignWorkers > 0.5f) {
		if (auto* worker = findReassignableWorker(order, prefs)) {
			freeWorkers.push_back(worker);
		}
	}

	if (freeWorkers.empty()) { return; }

	// Split workers across resources proportionally to their needs.
	std::array<int, 4> remaining = computeWorkerTargets(prefs, static_cast<int>(freeWorkers.size()));

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
// (non-negative) needs, using largest-remainder rounding so the targets sum to workerCount.
std::array<int, 4> AiOrchestrator::computeWorkerTargets(const float (&prefs)[4], int workerCount) const {
	std::array<int, 4> target = {0, 0, 0, 0};
	float weight[4];
	float total = 0.f;
	for (int i = 0; i < 4; ++i) {
		weight[i] = std::max(0.f, prefs[i]);
		total += weight[i];
	}
	if (total <= 0.f || workerCount <= 0) { return target; }

	float frac[4];
	int assigned = 0;
	for (int i = 0; i < 4; ++i) {
		float raw = weight[i] / total * static_cast<float>(workerCount);
		target[i] = static_cast<int>(std::floor(raw));
		frac[i] = raw - static_cast<float>(target[i]);
		assigned += target[i];
	}
	// Hand out the remaining workers to the largest fractional parts.
	for (int leftover = workerCount - assigned; leftover > 0; --leftover) {
		int best = 0;
		for (int i = 1; i < 4; ++i) { if (frac[i] > frac[best]) { best = i; } }
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
	auto orderType = static_cast<AiOrderType>(
		static_cast<uint8_t>(AiOrderType::COLLECT_RESOURCE_0) + resId);
	Game::getActionCenter()->addUnitAction(
			new IndividualOrder(worker, UnitAction::COLLECT, closest));
	history->addOrder(orderType, AiOrderResult::SUCCESS, 1);
	return true;
}

// Returns a busy worker collecting the lowest-priority resource that should move to a higher-priority one.
Unit* AiOrchestrator::findReassignableWorker(const std::array<int, 4>& order, const float (&prefs)[4]) const {
	int worstResId = order[3];
	if (prefs[order[0]] - prefs[worstResId] < 0.3f) { return nullptr; }
	for (auto* worker : possession->getWorkers()) {
		if (worker->getState() != UnitState::COLLECT) { continue; }
		auto* res = dynamic_cast<ResourceEntity*>(worker->getThingToInteract());
		if (res && res->getResourceId() == worstResId) { return worker; }
	}
	return nullptr;
}

std::vector<Unit*> AiOrchestrator::findFreeWorkers() const {
	std::vector<Unit*> freeWorkers;
	std::ranges::copy_if(possession->getWorkers(),
	                     std::back_inserter(freeWorkers),
	                     isFreeWorker);
	return freeWorkers;
}

Physical* AiOrchestrator::closestInRange(Unit* worker, int resourceId) {
	float prevRadius = -1.f;
	const auto env = Game::getEnvironment();
	for (const auto radius : {64.f, 128.f, 256.f}) {
		const auto list = env->getResources(worker->getPosition(), resourceId, radius, prevRadius);
		const auto closest = env->closestPhysical(worker->getMainGridIndex(), list, belowClose, false);
		if (closest) { return closest; }
		prevRadius = radius;
	}
	return nullptr;
}

UnitOrder* AiOrchestrator::unitOrderCollect(std::vector<Unit*>& workers, Physical* closest) const {
	if (workers.size() > 1) {
		return new GroupOrder(workers, UnitActionType::ORDER, castC(UnitAction::COLLECT), closest);
	}
	return new IndividualOrder(workers.at(0), UnitAction::COLLECT, closest);
}
