#include "Possession.h"
#include "Game.h"
#include "Resources.h"
#include "ai/PossessionMetric.h"
#include "database/DatabaseCache.h"
#include "utils/SpanUtils.h"
#include "math/VectorUtils.h"
#include "objects/PhysicalUtils.h"
#include "objects/building/Building.h"
#include "objects/building/ParentBuildingType.h"
#include "objects/resource/ResourceEntity.h"
#include "ai/MetricDefinitions.h"
#include "simulation/formation/Formation.h"
#include "simulation/formation/FormationManager.h"

Possession::Possession(unsigned short nation) {
	for (const auto building : Game::getDatabase()->getNation(nation)->buildings) {
		auto id = building->id;
		if (buildingsPerId.size() <= id) {
			buildingsPerId.resize(id + 1, nullptr);
		}
		buildingsPerId[id] = new std::vector<Building*>();
	}

	metric = new PossessionMetric();

	levelsSize = Urho3D::Max(Game::getDatabase()->getUnitLevels().size(),
	                         Game::getDatabase()->getBuildingLevels().size());

	levels = new float[levelsSize];
	std::fill_n(levels, levelsSize, 0.f);
}

Possession::~Possession() {
	clear_vector(buildingsPerId);
	delete metric;
	delete[]levels;
}

unsigned Possession::getScore() const {
	float buildingScore = 0.f;
	for (const auto building : buildings) {
		auto cost = building->getCostSum();

		if (!building->isReady()) {
			cost *= 0.5;
		}
		buildingScore += cost;
	}

	float unitsScore = 0.f;
	float workerScore = 0.f;
	for (const auto unit : units) {
		if (unit->getDb()->typeWorker) {
			workerScore += unit->getCostSum();
		} else {
			unitsScore += unit->getCostSum();
		}
	}

	return buildingScore * 0.25f + unitsScore * 0.2f + workerScore * 0.05f + resourcesSum * 0.01f;
	//return buildings.size() * 10 + units.size() + resourcesSum / 100;
}

unsigned Possession::getUnitsNumber() const {
	return units.size();
}

unsigned Possession::getBuildingsNumber() const {
	return buildings.size();
}

unsigned Possession::getWorkersNumber() const {
	return workers.size();
}

unsigned Possession::getFreeWorkersNumber() {
	ensureCounts();
	return idleWorkersNumber;
}

unsigned Possession::getFreeArmyNumber() {
	ensureCounts();
	return idleArmyNumber;
}

unsigned Possession::getArmyNumber() {
	ensureCounts();
	return armyNumber;
}

unsigned Possession::getInfantryNumber() {
	ensureCounts();
	return typeInfantryNumber;
}

unsigned Possession::getCavalryNumber() {
	ensureCounts();
	return typeCavalryNumber;
}

unsigned Possession::getMeleeNumber() {
	ensureCounts();
	return typeMeleeNumber;
}

unsigned Possession::getRangeNumber() {
	ensureCounts();
	return typeRangeNumber;
}

unsigned Possession::getLightNumber() {
	ensureCounts();
	return typeLightNumber;
}

unsigned Possession::getHeavyNumber() {
	ensureCounts();
	return typeHeavyNumber;
}

std::span<float> Possession::getResWithOutBonus() {
	ensureResWithoutBonus();
	return metric->resWithoutBonus;
}

float Possession::getResWithOutBonus(ResourceType rt) {
	ensureResWithoutBonus();
	return metric->resWithoutBonus[cast(rt)];
}

std::vector<Building*>* Possession::getBuildings(short id) {
	return buildingsPerId[id];
}

const std::vector<Building*>& Possession::getBuildings() {
	return buildings;
}

void Possession::addKilled(Physical* physical) {
	valueDestroyed += physical->getCostSum();
}

const PossessionMetric* Possession::getMetrics() {
	ensureUnitMetrics();
	ensureBuildingMetrics();
	ensureResWithoutBonus();
	return metric;
}

std::vector<Unit*> Possession::getFreeArmy() {
	std::vector<Unit*> army(units.size());

	const auto it = std::ranges::copy_if(units, army.begin(), isFreeSolider).out;
	army.resize(std::distance(army.begin(), it));
	return army;
}

std::vector<Unit*> Possession::getAllArmy() {
	std::vector<Unit*> army(units.size());

	const auto it = std::ranges::copy_if(units, army.begin(), isSolider).out;
	army.resize(std::distance(army.begin(), it));
	return army;
}

const std::vector<Unit*>& Possession::getArmyInAttack() {
	return getArmyByActivity().attacking;
}

const std::vector<Unit*>& Possession::getArmyInDefend() {
	return getArmyByActivity().defending;
}

const ArmyByActivity& Possession::getArmyByActivity() {
	armyByActivity.attacking.clear();
	armyByActivity.defending.clear();
	armyByActivity.idle.clear();

	const auto formationManager = Game::getFormationManager();

	for (const auto unit : units) {
		if (!isSolider(unit)) { continue; }

		bool attacking;
		bool defending;
		if (unit->getFormation() >= 0) {
			const auto formation = formationManager->getFormation(unit);
			attacking = formation->isInAttack();
			defending = formation->isInDefend();
		} else {
			attacking = isInAttack(unit);
			defending = isInDefend(unit);
		}

		if (attacking) {
			armyByActivity.attacking.push_back(unit);
		} else if (defending) {
			armyByActivity.defending.push_back(unit);
		} else {
			armyByActivity.idle.push_back(unit);
		}
	}
	return armyByActivity;
}

bool Possession::hasAnyFreeArmy() const {
	return std::ranges::any_of(units, isFreeSolider);
}

float Possession::getAttackSum() {
	ensureUnitMetrics();
	return metric->unitsSum[static_cast<unsigned char>(UnitMetricIdx::ATTACK)];
}

float Possession::getFreeArmyAttackSum() {
	ensureUnitMetrics();
	return freeArmyAttackSum;
}

float Possession::getDefenceAttackSum() {
	ensureBuildingMetrics();
	return metric->buildingsSum[static_cast<unsigned char>(BuildingMetricIdx::ATTACK)];
}

unsigned Possession::getResourceBuildingCount() {
	ensureBuildingMetrics();
	return resourceBuildingCount;
}

unsigned Possession::getConvertBuildingCount() {
	ensureBuildingMetrics();
	return convertBuildingCount;
}

unsigned Possession::getSpawnerCount() {
	ensureBuildingMetrics();
	return spawnerCount;
}

float Possession::getInCombatRatio() {
	ensureCounts();
	return armyNumber > 0 ? static_cast<float>(inCombatNumber) / static_cast<float>(armyNumber) : 0.f;
}

float Possession::getBonusCoverage(ResourceType rt) {
	ensureResWithoutBonus();
	return bonusCoverage[cast(rt)];
}

void Possession::add(Building* building) {
	buildings.push_back(building);
	buildingsPerId[building->getDbId()]->push_back(building);
}

void Possession::add(Unit* unit) {
	units.push_back(unit);

	if (unit->getDb()->typeWorker) {
		workers.push_back(unit);
	}
}

void Possession::updateAndClean(Resources* resources) {
	unitMetricsReady = false;
	buildingMetricsReady = false;
	countsReady = false;
	resWithoutBonusReady = false;
	cleanDead(buildings, StateManager::isBuildingDead());
	cleanDead(units, StateManager::isUnitDead());
	cleanDead(workers, StateManager::isUnitDead());

	if (StateManager::isBuildingDead()) {
		for (const auto perId : buildingsPerId) {
			if (perId) {
				cleanDead(perId);
			}
		}
	}
	resourcesSum = sumSpan(resources->getValues());
}

void Possession::ensureUnitMetrics() {
	if (unitMetricsReady) { return; }
	unitMetricsReady = true;

	resetSpan(metric->unitsSum);
	freeArmyAttackSum = 0.f;

	assert(zerosSpan(levels, levelsSize));

	for (const auto unit : units) {
		auto per = unit->getHealthPercent();
		levels[unit->getLevel()->id] += per;
	}
	constexpr auto attackIdx = static_cast<unsigned char>(UnitMetricIdx::ATTACK);
	auto& uLevels = Game::getDatabase()->getUnitLevels();
	for (int i = 0; i < levelsSize; ++i) {
		const auto val = levels[i];
		if (val > 0.f) {
			auto& metric1 = uLevels[i]->dbUnitMetric->getValuesNormForSum();
			assert(metric1.size() == metric->unitsSum.size());
			for (int j = 0; j < metric->unitsSum.size(); ++j) {
				metric->unitsSum[j] += val * metric1[j];
			}
			levels[i] = 0.f;
		}
	}

	for (const auto unit : units) {
		if (isFreeSolider(unit)) {
			auto& metric1 = uLevels[unit->getLevel()->id]->dbUnitMetric->getValuesNormForSum();
			freeArmyAttackSum += unit->getHealthPercent() * metric1[attackIdx];
		}
	}

	assert(validateSpan(__LINE__, __FILE__, metric->unitsSum));
}

void Possession::ensureBuildingMetrics() {
	if (buildingMetricsReady) { return; }
	buildingMetricsReady = true;

	resetSpan(metric->buildingsSum);

	zerosSpan(levels, levelsSize);

	resourceBuildingCount = 0;
	convertBuildingCount = 0;
	spawnerCount = 0;

	for (const auto building : buildings) {
		levels[building->getLevel()->id] += building->getHealthPercent();

		if (building->isReady()) {
			const auto db = building->getDb();
			if (db->typeResourceAny) { ++resourceBuildingCount; }
			if (db->toResource >= 0) {
				if (building->getLevel()->spawnResourceRange > 0) {
					++spawnerCount;
				} else {
					++convertBuildingCount;
				}
			}
		}
	}

	auto& bLevels = Game::getDatabase()->getBuildingLevels();
	for (int i = 0; i < levelsSize; ++i) {
		const auto lVal = levels[i];
		if (lVal > 0.f) {
			auto& metric1 = bLevels[i]->dbBuildingMetric->getValuesNormForSum();
			assert(metric1.size() == metric->buildingsSum.size());
			for (int j = 0; j < metric->buildingsSum.size(); ++j) {
				metric->buildingsSum[j] += lVal * metric1[j];
			}
			levels[i] = 0.f;
		}
	}

	assert(validateSpan(__LINE__, __FILE__, metric->buildingsSum));
}

void Possession::ensureCounts() {
	if (countsReady) { return; }
	countsReady = true;

	idleWorkersNumber = std::ranges::count_if(workers, isInFreeState);

	idleArmyNumber = 0;
	armyNumber = 0;
	inCombatNumber = 0;
	typeInfantryNumber = 0;
	typeCavalryNumber = 0;
	typeRangeNumber = 0;
	typeMeleeNumber = 0;
	typeHeavyNumber = 0;
	typeLightNumber = 0;
	typeSpecialNumber = 0;

	for (const auto unit : units) {
		if (isSolider(unit)) {
			++armyNumber;
			const auto dbUnit = unit->getDb();
			if (isFree(unit)) { ++idleArmyNumber; }
			auto state = unit->getState();
			if (state == UnitState::ATTACK || state == UnitState::SHOT
				|| state == UnitState::CHARGE || state == UnitState::DEFEND) {
				++inCombatNumber;
			}
			if (dbUnit->typeInfantry) { ++typeInfantryNumber; }
			if (dbUnit->typeCavalry) { ++typeCavalryNumber; }
			if (dbUnit->typeRange) { ++typeRangeNumber; }
			if (dbUnit->typeMelee) { ++typeMeleeNumber; }
			if (dbUnit->typeLight) { ++typeLightNumber; }
			if (dbUnit->typeHeavy) { ++typeHeavyNumber; }
			if (dbUnit->typeSpecial) { ++typeSpecialNumber; }
		}
	}
}

void Possession::ensureResWithoutBonus() {
	if (resWithoutBonusReady) { return; }
	resWithoutBonusReady = true;

	resetSpan(metric->resWithoutBonus);
	bonusCoverage.fill(0.f);
	float workerPerRes[4] = {0.f, 0.f, 0.f, 0.f};

	for (const auto worker : workers) {
		if (worker->getState() == UnitState::COLLECT && worker->isFirstThingAlive()) {
			auto res = (ResourceEntity*)worker->getThingToInteract();
			auto resId = res->getResourceId();
			workerPerRes[resId] += 1.f;

			const auto bonus = res->getBonus(worker->getPlayer());
			if (bonus > 1.f) {
				bonusCoverage[resId] += 1.f;
			} else {
				metric->resWithoutBonus[resId] += 1;
			}
		}
	}

	for (int i = 0; i < 4; ++i) {
		if (workerPerRes[i] > 0.f) {
			bonusCoverage[i] /= workerPerRes[i];
		}
	}
}
