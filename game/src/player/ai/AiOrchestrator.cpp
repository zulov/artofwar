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
	buildSpatialBrain(nation),
	attackSpatialBrain(nation) {
	lastLacking.reset();
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
		wantList.addRequest(WantItemType::WORKER, lastEconOut.workerAllocation, lastEconOut.workerCount);
	}

	// Unit request
	if (unitOut.count > 0) {
		auto units = resolveUnit(unitOut);
		for (auto* unit : units) {
			wantList.addRequest(WantItemType::UNIT, lastMasterOut.unitUrgency, 1, unit->id);
		}
	}

	// Unit upgrade request
	if (unitOut.unitUpgradeUrgency > 0.1f) {
		auto* unitToUpgrade = resolveUnitUpgrade(unitOut);
		if (unitToUpgrade) {
			wantList.addRequest(WantItemType::UNIT_UPGRADE, unitOut.unitUpgradeUrgency, 1, unitToUpgrade->id);
		}
	}

	// Unit-producing building upgrade request (barracks, archery range, stable)
	if (unitOut.buildingUpgradeUrgency > 0.1f) {
		auto* buildingToUpgrade = resolveBuildingUpgrade(unitOut);
		if (buildingToUpgrade) {
			wantList.addRequest(WantItemType::BUILDING_UPGRADE, unitOut.buildingUpgradeUrgency, 1, buildingToUpgrade->id);
		}
	}

	// Worker upgrade request
	if (lastEconOut.workerUpgradeUrgency > 0.1f) {
		auto* workerToUpgrade = resolveWorkerUpgrade();
		if (workerToUpgrade) {
			wantList.addRequest(WantItemType::UNIT_UPGRADE, lastEconOut.workerUpgradeUrgency, 1, workerToUpgrade->id);
		}
	}

	// Resource building upgrade request (farms, mills, mines, refineries, etc.)
	if (lastEconOut.resBuildingUpgradeUrgency > 0.1f) {
		auto* resBuildingToUpgrade = resolveResBuildingUpgrade(lastEconOut);
		if (resBuildingToUpgrade) {
			wantList.addRequest(WantItemType::BUILDING_UPGRADE, lastEconOut.resBuildingUpgradeUrgency, 1, resBuildingToUpgrade->id);
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
	{
		const auto buildings = getBuildingsInType(ParentBuildingType::RESOURCE);

		auto findAndSubmit = [&](float need, auto&& match) {
			if (need <= 0.1f) { return; }
			for (const auto b : buildings) {
				auto lvlOpt = b->getLevel(0);
				if (!lvlOpt.has_value()) { continue; }
				if (match(b, lvlOpt.value())) {
					wantList.addRequest(WantItemType::BUILDING, need, 1, b->id);
					return;
				}
			}
		};

		findAndSubmit(lastEconOut.needBonusFood,  [](auto* b, auto* l) { return isResBonus(b, l, ResourceType::FOOD); });
		findAndSubmit(lastEconOut.needBonusWood,  [](auto* b, auto* l) { return isResBonus(b, l, ResourceType::WOOD); });
		findAndSubmit(lastEconOut.needBonusStone, [](auto* b, auto* l) { return isResBonus(b, l, ResourceType::STONE); });
		findAndSubmit(lastEconOut.needBonusGold,  [](auto* b, auto* l) { return isResBonus(b, l, ResourceType::GOLD); });
		findAndSubmit(lastEconOut.needFoodSource, [](auto* b, auto* l) { return b->toResource >= 0 && l->spawnResourceRange <= 0; });
		findAndSubmit(lastEconOut.needWoodSource, [](auto* b, auto* l) { return b->toResource >= 0 && l->spawnResourceRange > 0; });
		findAndSubmit(lastEconOut.needFoodStorage,[](auto* b, auto* l) { return l->foodStorage > 0; });
		findAndSubmit(lastEconOut.needGoldStorage,[](auto* b, auto* l) { return l->goldStorage > 0; });
		findAndSubmit(lastEconOut.needStoneRefine,[](auto* b, auto* l) { return l->stoneRefineCapacity > 0.f; });
		findAndSubmit(lastEconOut.needGoldRefine, [](auto* b, auto* l) { return l->goldRefineCapacity > 0.f; });
	}

	// UNITS building: prefer specific building from lacking feedback, fall back to generic resolve
	if (lastLacking.lackingBuildingForUnit >= 0) {
		float boostedUrgency = std::max(lastMasterOut.unitUrgency, 0.5f);
		wantList.addRequest(WantItemType::BUILDING, boostedUrgency, 1, lastLacking.lackingBuildingForUnit);
	} else {
		submitBuildingRequest(lastMasterOut.unitUrgency, ParentBuildingType::UNITS);
	}

	// 5. Execute WantList with callbacks
	pendingLackingBuilding = -1;

	lastLacking = wantList.execute(player,
		[this](WantItem& item) { return executeWantItem(item); },
		[this](const WantItem& item) { return getWantItemCost(item); }
	);
	lastLacking.lackingBuildingForUnit = pendingLackingBuilding;
}

void AiOrchestrator::submitBuildingRequest(float urgency, ParentBuildingType type) {
	if (urgency > 0.1f) {
		db_building* building = resolveBuilding(type);
		if (building) {
			wantList.addRequest(WantItemType::BUILDING, urgency, 1, building->id);
		}
	}
}

void AiOrchestrator::submitBuildingUpgradeRequest(float urgency, ParentBuildingType type) {
	if (urgency <= 0.1f) { return; }
	for (auto* building : nation->buildings) {
		if (building->parentType[static_cast<int>(type)]
			&& player->getNextLevelForBuilding(building->id).has_value()) {
			wantList.addRequest(WantItemType::BUILDING_UPGRADE, urgency, 1, building->id);
		}
	}
}

bool AiOrchestrator::executeWantItem(WantItem& item) {
	switch (item.type) {
	case WantItemType::WORKER:
		return executeWorker();
	case WantItemType::UNIT:
		return executeUnit(item.specificId);
	case WantItemType::BUILDING:
		return executeBuilding(item.specificId);
	case WantItemType::UNIT_UPGRADE:
		return executeUnitUpgrade(item.specificId);
	case WantItemType::BUILDING_UPGRADE:
		return executeBuildingUpgrade(item.specificId);
	}
	return false;
}

const db_with_cost* AiOrchestrator::getWantItemCost(const WantItem& item) const {
	switch (item.type) {
	case WantItemType::WORKER:
		return nation->workers.empty() ? nullptr : nation->workers.at(0);
	case WantItemType::UNIT:
		if (item.specificId >= 0) {
			return Game::getDatabase()->getUnit(item.specificId);
		}
		return nullptr;
	case WantItemType::BUILDING:
		if (item.specificId >= 0) {
			return Game::getDatabase()->getBuilding(item.specificId);
		}
		return nullptr;
	case WantItemType::UNIT_UPGRADE:
		if (item.specificId >= 0) {
			auto nextLevel = player->getNextLevelForUnit(item.specificId);
			if (nextLevel.has_value()) {
				return nextLevel.value();
			}
		}
		return nullptr;
	case WantItemType::BUILDING_UPGRADE:
		if (item.specificId >= 0) {
			auto nextLevel = player->getNextLevelForBuilding(item.specificId);
			if (nextLevel.has_value()) {
				return nextLevel.value();
			}
		}
		return nullptr;
	}
	return nullptr;
}

void AiOrchestrator::order() {
	// Worker collection
	collectWorkers();

	const auto enemy = Game::getPlayersMan()->getEnemyFor(playerId);
	const auto& milOut = lastMilOut;

	// Get ALL army (free + busy) for stance-based control
	std::vector<Unit*> allArmy = possession->getAllArmy();
	if (allArmy.empty()) { return; }

	const int totalArmy = static_cast<int>(allArmy.size());

	int attackCount = roundToInt(std::max(0.f, milOut.attackRatio) * totalArmy);
	int defendCount = roundToInt(std::max(0.f, milOut.defendRatio) * totalArmy);

	// Clamp so attack + defend <= total
	if (attackCount + defendCount > totalArmy) {
		float sum = milOut.attackRatio + milOut.defendRatio;
		if (sum > 0.f) {
			attackCount = roundToInt((milOut.attackRatio / sum) * totalArmy);
			defendCount = totalArmy - attackCount;
		} else {
			attackCount = 0;
			defendCount = 0;
		}
	}

	// Resolve target positions
	auto spatialOut = attackSpatialBrain.decide(
		player, enemy, milOut,
		lastMasterOut.militaryUrgency, lastMasterOut.attackUrgency
	);

	auto attackPosOpt = [&]() -> std::optional<Urho3D::Vector2> {
		auto areas = Game::getEnvironment()->getAreas(
			playerId, std::span<const float>(spatialOut.weights), 1
		);
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

	std::vector<Unit*> defendGroup(remaining.begin(), remaining.begin() + std::min(defendCount, static_cast<int>(remaining.size())));

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
						? static_cast<UnitOrder*>(new GroupOrder(subArmy, UnitActionType::ORDER, castC(UnitAction::GO), target))
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
		if (milOut.attackStance < STANCE_RETREAT && defendPosOpt.has_value()) {
			// Retreat: pull back toward own base
			for (auto* unit : attackGroup) {
				Game::getActionCenter()->addUnitAction(
					new IndividualOrder(unit, UnitAction::GO, defendPosOpt.value())
				);
			}
		} else if (milOut.attackStance < STANCE_ADVANCE) {
			issueHold(attackGroup);
		} else {
			issueAdvance(attackGroup, attackPosOpt.value());
		}
		history->addOrder(AiOrderType::ATTACK_ECON, AiOrderResult::SUCCESS, static_cast<uint8_t>(attackGroup.size()));
	}

	// Issue orders for defend group
	if (!defendGroup.empty() && defendPosOpt.has_value()) {
		if (milOut.defendStance < STANCE_RETREAT) {
			// Disengage: stop all units, let them idle
			for (auto* unit : defendGroup) {
				Game::getActionCenter()->addUnitAction(
					new IndividualOrder(unit, UnitAction::STOP, unit->getPosition())
				);
			}
		} else if (milOut.defendStance < STANCE_ADVANCE) {
			issueHold(defendGroup);
		} else {
			issueAdvance(defendGroup, defendPosOpt.value());
		}
		history->addOrder(AiOrderType::DEFEND_ECON, AiOrderResult::SUCCESS, static_cast<uint8_t>(defendGroup.size()));
	}
}

// --- Execution callbacks ---

bool AiOrchestrator::executeWorker() {
	if (nation->workers.empty()) { return false; }
	auto* unit = nation->workers.at(0);
	Building* building = getBuildingToDeployWorker(unit);
	if (building) {
		Game::getActionCenter()->add(
			new BuildingActionCommand(building, BuildingActionType::UNIT_CREATE, unit->id));
		history->addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS);
		return true;
	}
	history->addAction(AiActionType::CREATE_WORKER, AiActionResult::NO_BUILDING_TO_DEPLOY);
	return false;
}

bool AiOrchestrator::executeUnit(short unitId) {
	if (unitId < 0) { return false; }
	auto* unit = Game::getDatabase()->getUnit(unitId);
	Building* building = getBuildingToDeploy(unit);
	if (building) {
		Game::getActionCenter()->add(
			new BuildingActionCommand(building, BuildingActionType::UNIT_CREATE, unit->id));
		history->addAction(AiActionType::CREATE_UNIT, AiActionResult::SUCCESS);
		return true;
	}
	pendingLackingBuilding = findBuildingTypeToDeploy(unitId);
	history->addAction(AiActionType::CREATE_UNIT, AiActionResult::NO_BUILDING_TO_DEPLOY);
	return false;
}

bool AiOrchestrator::executeBuilding(short buildingId) {
	if (buildingId < 0) { return false; }
	auto* building = Game::getDatabase()->getBuilding(buildingId);

	// Find which ParentBuildingType this building belongs to
	ParentBuildingType type = ParentBuildingType::OTHER;
	for (int i = 0; i < 5; ++i) {
		if (building->parentType[i]) {
			type = static_cast<ParentBuildingType>(i);
			break;
		}
	}

	AiActionType actionType;
	switch (type) {
	case ParentBuildingType::OTHER: actionType = AiActionType::CREATE_BUILDING_OTHER; break;
	case ParentBuildingType::DEFENCE: actionType = AiActionType::CREATE_BUILDING_DEFENCE; break;
	case ParentBuildingType::RESOURCE: actionType = AiActionType::CREATE_BUILDING_RESOURCE; break;
	case ParentBuildingType::TECH: actionType = AiActionType::CREATE_BUILDING_TECH; break;
	case ParentBuildingType::UNITS: actionType = AiActionType::CREATE_BUILDING_UNITS; break;
	default: actionType = AiActionType::NONE; break;
	}

	if (type == ParentBuildingType::RESOURCE) {
		if (sumSpan(possession->getResWithOutBonus()) < 0.5f) {
			history->addAction(actionType, AiActionResult::NO_RESOURCE_BONUS);
			return false;
		}
	}

	auto pos = findPosToBuild(building, type);
	if (pos.has_value()) {
		Game::getActionCenter()->addBuilding(building->id, pos.value(), playerId, false);
		history->addAction(actionType, AiActionResult::SUCCESS);
		return true;
	}
	history->addAction(actionType, AiActionResult::NO_POSITION_TO_BUILD);
	return false;
}

bool AiOrchestrator::executeUnitUpgrade(short unitId) {
	if (unitId < 0) { return false; }
	auto nextLevel = player->getNextLevelForUnit(unitId);
	if (!nextLevel.has_value()) {
		history->addAction(AiActionType::UPGRADE_UNIT, AiActionResult::NO_UPGRADE_AVAILABLE);
		return false;
	}

	// Find a building that can handle this unit's upgrade
	auto buildings = getBuildingsCanDeploy(unitId);
	if (!buildings.empty()) {
		Game::getActionCenter()->add(
			new BuildingActionCommand(buildings[0], BuildingActionType::UNIT_LEVEL, unitId));
		history->addAction(AiActionType::UPGRADE_UNIT, AiActionResult::SUCCESS);
		return true;
	}

	// No building available — signal to build one
	pendingLackingBuilding = findBuildingTypeToDeploy(unitId);
	history->addAction(AiActionType::UPGRADE_UNIT, AiActionResult::NO_BUILDING_TO_DEPLOY);
	return false;
}

bool AiOrchestrator::executeBuildingUpgrade(short buildingId) {
	if (buildingId < 0) { return false; }
	auto nextLevel = player->getNextLevelForBuilding(buildingId);
	if (!nextLevel.has_value()) {
		history->addAction(AiActionType::UPGRADE_BUILDING, AiActionResult::NO_UPGRADE_AVAILABLE);
		return false;
	}
	Game::getActionCenter()->add(
		new UpgradeCommand(playerId, buildingId, QueueActionType::BUILDING_LEVEL));
	history->addAction(AiActionType::UPGRADE_BUILDING, AiActionResult::SUCCESS);
	return true;
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
	for (auto unit : units) {
		if (!unit->typeWorker) {
			candidates.push_back(unit);
		}
	}
	if (candidates.empty()) { return {}; }

	std::valarray<float> center(unitOutput.unitProfile.data(), unitOutput.unitProfile.size());
	std::vector<float> diffs;
	diffs.reserve(candidates.size());
	for (const auto unit : candidates) {
		diffs.push_back(dist(center, player->getLevelForUnit(unit->id)->dbUnitMetric));
	}

	std::vector<db_unit*> result;
	result.reserve(unitOutput.count);
	for (const auto inx : lowestWithRand(diffs, unitOutput.count)) {
		result.push_back(candidates[inx]);
	}
	return result;
}

db_unit* AiOrchestrator::resolveUnitUpgrade(const UnitOutput& unitOutput) {
	auto& units = nation->units;
	std::vector<db_unit*> candidates;
	candidates.reserve(units.size());
	for (auto unit : units) {
		if (!unit->typeWorker && player->getNextLevelForUnit(unit->id).has_value()) {
			candidates.push_back(unit);
		}
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
			&& player->getNextLevelForBuilding(building->id).has_value()) {
			candidates.push_back(building);
		}
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
	for (auto* worker : nation->workers) {
		if (player->getNextLevelForUnit(worker->id).has_value()) {
			return worker;
		}
	}
	return nullptr;
}

db_building* AiOrchestrator::resolveResBuildingUpgrade(const EconomyOutput& econOutput) {
	auto& buildings = nation->buildings;
	std::vector<db_building*> candidates;
	candidates.reserve(buildings.size());
	for (auto* building : buildings) {
		if (building->parentType[static_cast<int>(ParentBuildingType::RESOURCE)]
			&& player->getNextLevelForBuilding(building->id).has_value()) {
			candidates.push_back(building);
		}
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
		if (building->toResource >= 0 && level->spawnResourceRange <= 0) { weight += std::max(0.f, econOutput.needFoodSource); }
		if (building->toResource >= 0 && level->spawnResourceRange > 0) { weight += std::max(0.f, econOutput.needWoodSource); }

		weights.push_back(weight);
	}

	float totalWeight = 0.f;
	for (float w : weights) { totalWeight += w; }
	return candidates[sampleWeighted(weights, totalWeight)];
}

Building* AiOrchestrator::getBuildingToDeploy(db_unit* unit) const {
	std::vector<Building*> allPossible = getBuildingsCanDeploy(unit->id);
	if (allPossible.empty()) { return nullptr; }
	if (allPossible.size() == 1) { return allPossible[0]; }

	// Pick building closest to army center — deploy near the action
	auto armyCenter = Game::getEnvironment()->getCenterOf(CenterType::ARMY, playerId);
	if (!armyCenter.has_value()) { return allPossible[0]; }

	auto target = armyCenter.value();
	Building* best = allPossible[0];
	float bestDist = std::numeric_limits<float>::max();
	for (auto* b : allPossible) {
		float d = b->getPosition().SqDistXZ(target);
		if (d < bestDist) {
			bestDist = d;
			best = b;
		}
	}
	return best;
}

Building* AiOrchestrator::getBuildingToDeployWorker(db_unit* unit) const {
	std::vector<Building*> allPossible = getBuildingsCanDeploy(unit->id);
	if (allPossible.empty()) { return nullptr; }
	if (allPossible.size() == 1) { return allPossible[0]; }

	// Pick building closest to economy center — deploy near resources
	auto econCenter = Game::getEnvironment()->getCenterOf(CenterType::ECON, playerId);
	if (!econCenter.has_value()) { return allPossible[0]; }

	auto target = econCenter.value();
	Building* best = allPossible[0];
	float bestDist = std::numeric_limits<float>::max();
	for (auto* b : allPossible) {
		float d = b->getPosition().SqDistXZ(target);
		if (d < bestDist) {
			bestDist = d;
			best = b;
		}
	}
	return best;
}

std::vector<Building*> AiOrchestrator::getBuildingsCanDeploy(unsigned short unitId) const {
	const auto& buildings = nation->buildings;
	std::vector<short> buildingIdsThatCanDeploy;
	for (const auto building : buildings) {
		auto unitIds = player->getLevelForBuilding(building->id)->unitsPerNationIds[player->getNation()];
		if (std::ranges::find(*unitIds, unitId) != unitIds->end()) {
			buildingIdsThatCanDeploy.push_back(building->id);
		}
	}
	std::vector<Building*> allPossible;
	for (auto thatCanDeploy : buildingIdsThatCanDeploy) {
		std::ranges::copy_if(*possession->getBuildings(thatCanDeploy),
		                     std::back_inserter(allPossible), [](Building* b) { return b->isReady(); });
	}
	return allPossible;
}

short AiOrchestrator::findBuildingTypeToDeploy(short unitId) const {
	for (const auto building : nation->buildings) {
		auto unitIds = player->getLevelForBuilding(building->id)->unitsPerNationIds[player->getNation()];
		if (std::ranges::find(*unitIds, unitId) != unitIds->end()) {
			return building->id;
		}
	}
	return -1;
}

// --- Building resolution ---

db_building* AiOrchestrator::resolveBuilding(ParentBuildingType type) {
	const auto buildings = getBuildingsInType(type);
	if (buildings.empty()) { return nullptr; }
	if (buildings.size() == 1) { return buildings.at(0); }

	// TODO: use building metric matching with brain output
	return buildings.at(0);
}

std::optional<Urho3D::Vector2> AiOrchestrator::findPosToBuild(db_building* building, ParentBuildingType type) {
	if (type == ParentBuildingType::RESOURCE) {
		return Game::getEnvironment()->getPosToCreateResBonus(building, playerId);
	}
	// Use BuildSpatialBrain to compute influence map weights
	const auto enemy = Game::getPlayersMan()->getEnemyFor(playerId);
	auto spatialOut = buildSpatialBrain.decide(
		player, enemy,
		lastMasterOut.buildingUrgency, lastMasterOut.expandUrgency, lastMasterOut.defenceBuildingUrgency
	);
	return Game::getEnvironment()->getPosToCreate(
		std::span<const float>(spatialOut.weights), type, building, playerId);
}

Building* AiOrchestrator::getBuildingClosestArea(std::vector<Building*>& allPossible,
                                                  std::span<const float> weights, int minAreas) const {
	auto centers = Game::getEnvironment()->getAreas(playerId, weights, minAreas);
	float closestVal = std::numeric_limits<float>::max();
	Building* closest = allPossible[0];
	for (const auto possible : allPossible) {
		const Urho3D::Vector2& pos = possible->getPosition();
		for (auto& center : centers) {
			auto diff = pos - center;
			auto val = diff.LengthSquared();
			if (val < closestVal) {
				closest = possible;
				closestVal = val;
			}
		}
	}
	return closest;
}

std::vector<db_building*> AiOrchestrator::getBuildingsInType(ParentBuildingType type) {
	std::vector<db_building*> buildings;
	for (auto dbBuilding : nation->buildings) {
		if (dbBuilding->parentType[castC(type)]) {
			buildings.push_back(dbBuilding);
		}
	}
	return buildings;
}

float AiOrchestrator::dist(std::valarray<float>& center, const db_basic_metric* metric) {
	return sumSquaredError(center, metric->getValuesNormAsVal());
}

float AiOrchestrator::dist(std::valarray<float>& center, const db_building_metric* metric, ParentBuildingType type) {
	return sumSquaredError(center, metric->getValuesNormAsValForType(type));
}

bool AiOrchestrator::enoughResources(const db_with_cost* withCosts) const {
	return withCosts && player->getResources()->hasEnough(withCosts);
}

// --- Worker collection ---

void AiOrchestrator::collectWorkers() {
	auto freeWorkers = findFreeWorkers();

	// Use economy brain's resource priorities to decide which resource to collect
	float prefs[] = {lastEconOut.foodPriority, lastEconOut.woodPriority,
	                 lastEconOut.stonePriority, lastEconOut.goldPriority};
	std::array<int, 4> order = {0, 1, 2, 3};
	std::ranges::sort(order, [&](int a, int b) { return prefs[a] > prefs[b]; });

	// Reassign one busy worker from lowest-priority resource to highest
	if (lastEconOut.reassignWorkers > 0.5f) {
		bool found = false;
		for (int i = 3; i >= 0 && !found; --i) {
			int worstResId = order[i];
			if (prefs[order[0]] - prefs[worstResId] < 0.3f) { break; }
			for (auto* worker : possession->getWorkers()) {
				if (worker->getState() != UnitState::COLLECT) { continue; }
				auto* res = dynamic_cast<ResourceEntity*>(worker->getThingToInteract());
				if (!res || res->getResourceId() != worstResId) { continue; }
				freeWorkers.push_back(worker);
				found = true;
				break;
			}
		}
	}

	if (freeWorkers.empty()) { return; }

	// For each free worker, assign to highest-priority resource that has a nearby source
	for (auto* worker : freeWorkers) {

		bool assigned = false;
		for (int resId : order) {
			auto* closest = closestInRange(worker, resId);
			if (closest) {
				auto orderType = static_cast<AiOrderType>(
					static_cast<uint8_t>(AiOrderType::COLLECT_RESOURCE_0) + resId);
				Game::getActionCenter()->addUnitAction(
					new IndividualOrder(worker, UnitAction::COLLECT, closest));
				history->addOrder(orderType, AiOrderResult::SUCCESS, 1);
				assigned = true;
				break;
			}
		}
		if (!assigned) {
			auto failType = static_cast<AiOrderType>(
				static_cast<uint8_t>(AiOrderType::COLLECT_RESOURCE_0) + order[0]);
			history->addOrder(failType, AiOrderResult::NO_RESOURCE_IN_RANGE, 1);
		}
	}
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
		if (closest) {
			return closest;
		}
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
