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
	lastEconOut = economyBrain.decide(
		player, enemy,
		lastLacking.perResource,
		lastMasterOut.economyUrgency, lastMasterOut.workerUrgency, lastMasterOut.expandUrgency,
		history
	);

	// 3. Unit Brain
	auto unitOut = unitBrain.decide(
		player, enemy,
		lastMasterOut.unitUrgency, lastMasterOut.attackUrgency
	);

	// 4. Submit requests to WantList
	wantList.resetRequests();

	// Worker request
	if (lastEconOut.workerCount > 0) {
		wantList.addRequest(WantItemType::WORKER, lastEconOut.workerAllocation, lastEconOut.workerCount);
	}

	// Unit request
	if (unitOut.count > 0) {
		db_unit* unit = resolveUnit(unitOut);
		if (unit) {
			wantList.addRequest(WantItemType::UNIT, lastMasterOut.unitUrgency, unitOut.count, unit->id);
		}
	}

	// Building requests — use MasterBrain urgencies directly
	submitBuildingRequest(lastMasterOut.buildingUrgency, ParentBuildingType::OTHER);
	submitBuildingRequest(lastMasterOut.defenceBuildingUrgency, ParentBuildingType::DEFENCE);
	submitBuildingRequest(lastMasterOut.techUrgency, ParentBuildingType::TECH);

	// Resource building — urgency derived from highest need score
	float resBuildUrgency = std::max({lastEconOut.needMill, lastEconOut.needSawmill,
		lastEconOut.needMineS, lastEconOut.needMineG, lastEconOut.needFarm,
		lastEconOut.needGranary, lastEconOut.needBank, lastEconOut.needGoldRefinery,
		lastEconOut.needStoneRefinery, lastEconOut.needTreeNursery});
	submitBuildingRequest(resBuildUrgency, ParentBuildingType::RESOURCE);

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

bool AiOrchestrator::executeWantItem(WantItem& item) {
	switch (item.type) {
	case WantItemType::WORKER:
		return executeWorker();
	case WantItemType::UNIT:
		return executeUnit(item.specificId);
	case WantItemType::BUILDING:
		return executeBuilding(item.specificId);
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
	}
	return nullptr;
}

void AiOrchestrator::order() {
	// Worker collection
	collectWorkers();

	// Military orders via MilitaryBrain
	if (!possession->hasAnyFreeArmy()) { return; }

	const auto enemy = Game::getPlayersMan()->getEnemyFor(playerId);

	auto milOut = militaryBrain.decide(
		player, enemy,
		lastMasterOut.militaryUrgency, lastMasterOut.attackUrgency,
		history
	);

	bool isAttack = milOut.attackWeight > milOut.defendWeight;
	char playerToGo = isAttack ? enemy->getId() : playerId;

	auto targetToCenterType = [](float target) -> CenterType {
		if (target < 0.33f) { return CenterType::ECON; }
		if (target < 0.67f) { return CenterType::BUILDING; }
		return CenterType::ARMY;
	};

	CenterType centerType = targetToCenterType(isAttack ? milOut.attackTarget : milOut.defendTarget);

	auto orderType = AiOrderType::NONE;
	if (isAttack) {
		orderType = static_cast<AiOrderType>(
			static_cast<uint8_t>(AiOrderType::ATTACK_ECON) + static_cast<uint8_t>(centerType));
	} else {
		orderType = static_cast<AiOrderType>(
			static_cast<uint8_t>(AiOrderType::DEFEND_ECON) + static_cast<uint8_t>(centerType));
	}

	const auto posOpt = [&]() -> std::optional<Urho3D::Vector2> {
		auto spatialOut = attackSpatialBrain.decide(
			player, enemy, milOut,
			lastMasterOut.militaryUrgency, lastMasterOut.attackUrgency
		);
		auto areas = Game::getEnvironment()->getAreas(
			playerId, std::span<const float>(spatialOut.weights), 1
		);
		if (!areas.empty()) {
			return areas[0];
		}
		return Game::getEnvironment()->getCenterOf(centerType, playerToGo);
	}();
	if (!posOpt.has_value()) {
		history->addOrder(orderType, AiOrderResult::NO_CENTER_POSITION);
		return;
	}

	constexpr float SEMI_CLOSE = 30.f;
	constexpr float SQ_SEMI_CLOSE = SEMI_CLOSE * SEMI_CLOSE;

	std::vector<Unit*> army = possession->getFreeArmy();
	auto target = posOpt.value();
	for (auto& subArmy : divide(army)) {
		auto armyCenter = computeCenter(subArmy);
		if (armyCenter.SqDistXZ(target) > SQ_SEMI_CLOSE) {
			Game::getActionCenter()->addUnitAction(
				subArmy.size() > 1
					? static_cast<UnitOrder*>(new GroupOrder(subArmy, UnitActionType::ORDER, castC(UnitAction::GO), target))
					: static_cast<UnitOrder*>(new IndividualOrder(subArmy.at(0), UnitAction::GO, target))
			);
		} else {
			const auto unit = subArmy.at(0);
			std::vector<Physical*>* things = nullptr;
			std::vector<Physical*> tempThings;
			if (centerType == CenterType::BUILDING) {
				things = Game::getEnvironment()->getBuildingsFromTeamNotEq(unit, -1, SEMI_CLOSE);
			} else if (centerType == CenterType::ECON) {
				const auto neights = Game::getEnvironment()->getNeighboursFromTeamNotEq(unit, SEMI_CLOSE);
				tempThings.reserve(neights->size());
				std::ranges::copy_if(*neights, std::back_inserter(tempThings), isWorker);
				things = &tempThings;
			} else {
				things = Game::getEnvironment()->getNeighboursFromTeamNotEq(unit, SEMI_CLOSE);
			}
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
	history->addOrder(orderType, AiOrderResult::SUCCESS, static_cast<uint8_t>(army.size()));
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

// --- Unit resolution ---

db_unit* AiOrchestrator::resolveUnit(const UnitOutput& unitOutput) {
	auto& units = nation->units;
	std::vector<db_unit*> candidates;
	candidates.reserve(units.size());
	for (auto unit : units) {
		if (!unit->typeWorker) {
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

	const auto inx = lowestWithRand(diffs);
	return candidates[inx];
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

	if (type == ParentBuildingType::RESOURCE) {
		// 10 building-type need scores from EconomyBrain
		// Each maps to a capability+resource combination
		struct BuildingNeed {
			float need;
			// Classification criteria (matched against db_building + level 0)
			int resourceType;     // -1 = any
			bool wantBonus;       // collect > 0 && resourceRange > 0
			bool wantConvert;     // toResource >= 0 && spawnResourceRange <= 0
			bool wantStorage;     // foodStorage > 0 || goldStorage > 0
			bool wantRefine;      // stoneRefineCapacity > 0 || goldRefineCapacity > 0
			bool wantSpawn;       // toResource >= 0 && spawnResourceRange > 0
			// For storage/refine disambiguation
			bool wantFoodStorage; // foodStorage > 0
			bool wantGoldStorage; // goldStorage > 0
			bool wantStoneRefine; // stoneRefineCapacity > 0
			bool wantGoldRefine;  // goldRefineCapacity > 0
		};

		BuildingNeed needs[] = {
			{lastEconOut.needMill,           0, true,  false, false, false, false, false, false, false, false},
			{lastEconOut.needSawmill,        1, true,  false, false, false, false, false, false, false, false},
			{lastEconOut.needMineS,          2, true,  false, false, false, false, false, false, false, false},
			{lastEconOut.needMineG,          3, true,  false, false, false, false, false, false, false, false},
			{lastEconOut.needFarm,          -1, false, true,  false, false, false, false, false, false, false},
			{lastEconOut.needGranary,       -1, false, false, true,  false, false, true,  false, false, false},
			{lastEconOut.needBank,          -1, false, false, true,  false, false, false, true,  false, false},
			{lastEconOut.needGoldRefinery,  -1, false, false, false, true,  false, false, false, false, true},
			{lastEconOut.needStoneRefinery, -1, false, false, false, true,  false, false, false, true,  false},
			{lastEconOut.needTreeNursery,   -1, false, false, false, false, true,  false, false, false, false},
		};

		// Sort needs descending by score
		constexpr int NEED_COUNT = 10;
		int order[NEED_COUNT] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		std::sort(order, order + NEED_COUNT, [&](int a, int b) { return needs[a].need > needs[b].need; });

		// For each need (highest first), find a matching building
		for (int i = 0; i < NEED_COUNT; ++i) {
			const auto& n = needs[order[i]];
			if (n.need <= 0.f) { break; }

			for (const auto building : buildings) {
				auto levelOpt = building->getLevel(0);
				if (!levelOpt.has_value()) { continue; }
				auto* lvl = levelOpt.value();

				// Resource type check
				if (n.resourceType >= 0 && building->resourceType != n.resourceType) { continue; }

				bool isBonus = lvl->collect > 0.f && lvl->resourceRange > 0.f;
				bool isConvert = building->toResource >= 0 && lvl->spawnResourceRange <= 0;
				bool isSpawn = building->toResource >= 0 && lvl->spawnResourceRange > 0;
				bool hasFoodStorage = lvl->foodStorage > 0;
				bool hasGoldStorage = lvl->goldStorage > 0;
				bool hasStoneRefine = lvl->stoneRefineCapacity > 0.f;
				bool hasGoldRefine = lvl->goldRefineCapacity > 0.f;

				if (n.wantBonus && !isBonus) { continue; }
				if (n.wantConvert && !isConvert) { continue; }
				if (n.wantSpawn && !isSpawn) { continue; }
				if (n.wantFoodStorage && !hasFoodStorage) { continue; }
				if (n.wantGoldStorage && !hasGoldStorage) { continue; }
				if (n.wantStoneRefine && !hasStoneRefine) { continue; }
				if (n.wantGoldRefine && !hasGoldRefine) { continue; }

				return building;
			}
		}
		return nullptr;
	}

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
