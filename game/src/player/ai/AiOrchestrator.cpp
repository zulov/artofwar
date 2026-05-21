#include "AiOrchestrator.h"

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
#include "env/influence/CenterType.h"
#include "database/DatabaseCache.h"


AiOrchestrator::AiOrchestrator(Player* player, db_nation* nation, AiHistory* history) :
	player(player), playerId(player->getId()), possession(player->getPossession()), nation(nation),
	history(history),
	masterBrain(nation),
	economyBrain(nation),
	buildingBrain(nation),
	unitBrain(nation),
	militaryBrain(nation) {
	lastLacking.perResource.fill(0.f);
	lastLacking.totalSum = 0.f;
}

void AiOrchestrator::action() {
	const auto enemy = Game::getPlayersMan()->getEnemyFor(playerId);

	// 1. Master Brain decides urgencies
	lastMasterOut = masterBrain.decide(
		player, enemy,
		std::span<const float>(lastLacking.perResource),
		lastLacking.totalSum
	);

	// 2. Economy Brain (gets lacking feedback)
	lastEconOut = economyBrain.decide(
		player, enemy,
		std::span<const float>(lastLacking.perResource),
		lastMasterOut.economyUrgency, lastMasterOut.workerUrgency, lastMasterOut.expandUrgency
	);

	// 3. Building Brain
	lastBuildOut = buildingBrain.decide(
		player, enemy,
		lastMasterOut.buildingUrgency, lastMasterOut.techUrgency, lastMasterOut.defenceBuildingUrgency
	);

	// 4. Unit Brain
	auto unitOut = unitBrain.decide(
		player, enemy,
		lastMasterOut.unitUrgency, lastMasterOut.attackUrgency
	);

	// 5. Submit requests to WantList
	wantList.beginTick();

	// Worker request
	if (lastEconOut.workerAllocation > 0.1f) {
		wantList.addRequest(WantItemType::WORKER, lastEconOut.workerAllocation);
	}

	// Unit request
	if (unitOut.count > 0) {
		db_unit* unit = resolveUnit(unitOut);
		if (unit) {
			wantList.addRequest(WantItemType::UNIT, lastMasterOut.unitUrgency, unitOut.count, unit->id);
		}
	}

	// Building requests
	auto submitBuilding = [&](float urgency, ParentBuildingType type) {
		if (urgency > 0.1f) {
			db_building* building = resolveBuilding(lastBuildOut, type);
			if (building) {
				wantList.addRequest(WantItemType::BUILDING, urgency, 1, building->id);
			}
		}
	};
	submitBuilding(lastBuildOut.otherUrgency, ParentBuildingType::OTHER);
	submitBuilding(lastBuildOut.defenceUrgency, ParentBuildingType::DEFENCE);
	submitBuilding(lastBuildOut.resourceUrgency, ParentBuildingType::RESOURCE);
	submitBuilding(lastBuildOut.techUrgency, ParentBuildingType::TECH);
	submitBuilding(lastBuildOut.unitsUrgency, ParentBuildingType::UNITS);

	// 6. Execute WantList with callbacks
	auto executeFn = [this](WantItem& item) -> bool {
		switch (item.type) {
		case WantItemType::WORKER:
			return executeWorker();
		case WantItemType::UNIT:
			return executeUnit(item.specificId);
		case WantItemType::BUILDING:
			return executeBuilding(item.specificId);
		}
		return false;
	};

	auto costFn = [this](const WantItem& item) -> const db_with_cost* {
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
	};

	lastLacking = wantList.execute(player, executeFn, costFn);
}

void AiOrchestrator::order() {
	// Worker collection
	collectWorkers();

	// Military orders via MilitaryBrain
	if (!possession->hasAnyFreeArmy()) { return; }

	const auto enemy = Game::getPlayersMan()->getEnemyFor(playerId);

	auto milOut = militaryBrain.decide(
		player, enemy,
		lastMasterOut.militaryUrgency, lastMasterOut.attackUrgency
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

	const auto posOpt = Game::getEnvironment()->getCenterOf(centerType, playerToGo);
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

	std::valarray<float> center(unitOutput.unitProfile.data(), 24);
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
	return allPossible.at(0);
	// TODO: spatial brain for placement
}

Building* AiOrchestrator::getBuildingToDeployWorker(db_unit* unit) const {
	std::vector<Building*> allPossible = getBuildingsCanDeploy(unit->id);
	if (allPossible.empty()) { return nullptr; }
	return allPossible.at(0);
}

std::vector<Building*> AiOrchestrator::getBuildingsCanDeploy(short unitId) const {
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

// --- Building resolution ---

db_building* AiOrchestrator::resolveBuilding(const BuildingOutput& output, ParentBuildingType type) {
	const auto buildings = getBuildingsInType(type);
	if (buildings.empty()) { return nullptr; }
	if (buildings.size() == 1) { return buildings.at(0); }

	if (type == ParentBuildingType::RESOURCE) {
		float prefs[] = {output.foodResPref, output.woodResPref, output.stoneResPref, output.goldResPref};
		int bestRes = static_cast<int>(std::max_element(prefs, prefs + 4) - prefs);
		for (const auto building : buildings) {
			if (building->resourceType == bestRes) { return building; }
		}
		return nullptr;
	}

	// TODO: use building metric matching with brain output
	return buildings.at(0);
}

std::optional<Urho3D::Vector2> AiOrchestrator::findPosToBuild(db_building* building, ParentBuildingType type) const {
	if (type == ParentBuildingType::RESOURCE) {
		return Game::getEnvironment()->getPosToCreateResBonus(building, playerId);
	}
	// TODO: use spatial brain for placement
	// For now, use a neutral placement result (zeros = center bias)
	std::array<float, 4> defaultResult{};
	defaultResult.fill(0.5f);
	return Game::getEnvironment()->getPosToCreate(
		std::span<const float>(defaultResult), type, building, playerId);
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
	return sqRootSumError(center, metric->getValuesNormAsVal());
}

float AiOrchestrator::dist(std::valarray<float>& center, const db_building_metric* metric, ParentBuildingType type) {
	return sqRootSumError(center, metric->getValuesNormAsValForType(type));
}

bool AiOrchestrator::enoughResources(const db_with_cost* withCosts) const {
	return withCosts && player->getResources()->hasEnough(withCosts);
}

// --- Worker collection ---

void AiOrchestrator::collectWorkers() {
	auto freeWorkers = findFreeWorkers();
	if (freeWorkers.empty()) { return; }

	// Use economy brain's resource priorities to decide which resource to collect
	float prefs[] = {lastEconOut.foodPriority, lastEconOut.woodPriority,
	                 lastEconOut.stonePriority, lastEconOut.goldPriority};

	// For each free worker, assign to highest-priority resource that has a nearby source
	for (auto* worker : freeWorkers) {
		// Try resources in priority order
		std::array<int, 4> order = {0, 1, 2, 3};
		std::ranges::sort(order, [&](int a, int b) { return prefs[a] > prefs[b]; });

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
			history->addOrder(AiOrderType::COLLECT_RESOURCE_0, AiOrderResult::NO_RESOURCE_IN_RANGE, 1);
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
