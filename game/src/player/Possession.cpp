#include "Possession.h"
#include "Game.h"
#include "Resources.h"
#include "ai/PossessionMetric.h"
#include "database/DatabaseCache.h"
#include "math/SpanUtils.h"
#include "math/VectorUtils.h"
#include "objects/PhysicalUtils.h"
#include "objects/building/Building.h"
#include "objects/building/ParentBuildingType.h"
#include "objects/resource/ResourceEntity.h"

Possession::Possession(char nation) {
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
	levelsFree = new float[levelsSize];
	std::fill_n(levels, levelsSize, 0.f);
	std::fill_n(levelsFree, levelsSize, 0.f);
}

Possession::~Possession() {
	clear_vector(buildingsPerId);
	delete metric;
	delete[]levels;
	delete[]levelsFree;
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
	ensureReady();
	return idleWorkersNumber;
}

unsigned Possession::getFreeArmyNumber() {
	ensureReady();
	return idleArmyNumber;
}

unsigned Possession::getArmyNumber() {
	ensureReady();
	return armyNumber;
}

unsigned Possession::getInfantryNumber() {
	ensureReady();
	return typeInfantryNumber;
}

unsigned Possession::getCalvaryNumber() {
	ensureReady();
	return typeCalvaryNumber;
}

unsigned Possession::getMeleeNumber() {
	ensureReady();
	return typeMeleeNumber;
}

unsigned Possession::getRangeNumber() {
	ensureReady();
	return typeRangeNumber;
}

unsigned Possession::getLightNumber() {
	ensureReady();
	return typeLightNumber;
}

unsigned Possession::getHeavyNumber() {
	ensureReady();
	return typeHeavyNumber;
}

std::span<float> Possession::getResWithOutBonus() {
	ensureReady();
	return metric->resWithoutBonus;
}

std::vector<Building*>* Possession::getBuildings(short id) {
	return buildingsPerId[id];
}

const std::vector<Building*>& Possession::getBuildings() {
	return buildings;
}

void Possession::addKilled(Physical* physical) {
	resourcesDestroyed += physical->getCostSum();
}

std::span<float> Possession::getUnitsMetrics() {
	ensureReady();
	return metric->unitsSum;
}

std::span<float> Possession::getFreeArmyMetrics() {
	ensureReady();
	return metric->freeArmySum;
}

std::span<float> Possession::getBuildingsMetrics() {
	ensureReady();
	return metric->buildingsSum;
}

std::span<float> Possession::refreshBuildingSum(const std::span<const unsigned char> idxs, std::span<float> out) const {
	assert(idxs.size() == out.size());
	for (int i = 0; i < idxs.size(); ++i) {
		out[i] = metric->buildingsSum[idxs[i]];
	}
	return out;
}

std::span<float> Possession::getBuildingsMetrics(ParentBuildingType type) {
	ensureReady();
	switch (type) {
	case ParentBuildingType::OTHER:
		return refreshBuildingSum(METRIC_DEFINITIONS.getBuildingOtherIdxs(), metric->buildingsOther);
	case ParentBuildingType::DEFENCE:
		return refreshBuildingSum(METRIC_DEFINITIONS.getBuildingDefenceIdxs(), metric->buildingsDefence);
	case ParentBuildingType::RESOURCE:
		assert(false);
	//return refreshResource(METRIC_DEFINITIONS.getResWithoutBonusIdxs(), buildingsResSumSpan);
	case ParentBuildingType::TECH:
		return refreshBuildingSum(METRIC_DEFINITIONS.getBuildingTechIdxs(), metric->buildingsTech);
	case ParentBuildingType::UNITS:
		return refreshBuildingSum(METRIC_DEFINITIONS.getBuildingUnitsIdxs(), metric->buildingsUnits);
	default: ;
	}
}

std::vector<Unit*> Possession::getFreeArmy() {
	std::vector<Unit*> army(units.size());

	const auto it = std::ranges::copy_if(units, army.begin(), isFreeSolider).out;
	army.resize(std::distance(army.begin(), it));
	return army;
}

bool Possession::hasAnyFreeArmy() const {
	return std::ranges::any_of(units, isFreeSolider);
}

float Possession::getAttackSum() {
	ensureReady();
	return metric->unitsSum[5];
	//TODO hardcoded from AiUnitMetric {[](db_unit* u, db_unit_level* l) -> float { return l->attack; }, 10, UNITS_SUM_X},
}

float Possession::getDefenceAttackSum() {
	ensureReady();
	return metric->buildingsSum[5];

	//TODO hardcoded from AiBuildingMetric {[](db_building* b, db_building_level* l) -> float { return l->attack; }, 20, BUILDINGS_SUM_X},
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
	ready = false;
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

void Possession::ensureReady() {
	if (ready) { return; }
	ready = true;
	resetSpan(metric->unitsSum);
	resetSpan(metric->freeArmySum);

	assert(zerosSpan(levels, levelsSize));
	assert(zerosSpan(levelsFree, levelsSize));

	for (const auto unit : units) {
		auto per = unit->getHealthPercent();
		levels[unit->getLevel()->id] += per;//ID to -1

		//TODO to dac jako któtkie
		if (isFreeSolider(unit)) {
			levelsFree[unit->getLevel()->id] += per;
		}
	}
	auto& uLevels = Game::getDatabase()->getUnitLevels();
	for (int i = 0; i < levelsSize; ++i) {
		const auto val = levels[i];
		if (val > 0.f) {
			auto& metric1 = uLevels[i]->dbUnitMetric->getValuesNormForSum();
			assert(metric1.size() == metric->unitsSum.size());
			for (int j = 0; j < metric->unitsSum.size(); ++j) {
				metric->unitsSum[j] += val * metric1[j];
			}
			const auto val2 = levelsFree[i];
			if (val2 > 0.f) {
				for (int j = 0; j < metric->freeArmySum.size(); ++j) {
					metric->freeArmySum[j] += val2 * metric1[j];
				}
				levelsFree[i] = 0.f;
			}
			levels[i] = 0.f;
		}
	}

	resetSpan(metric->buildingsSum);

	zerosSpan(levels, levelsSize);
	zerosSpan(levelsFree, levelsSize);

	for (const auto building : buildings) {
		levels[building->getLevel()->id] += building->getHealthPercent();
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

	idleWorkersNumber = std::ranges::count_if(workers, isInFreeState);

	idleArmyNumber = 0;
	armyNumber = 0;
	typeInfantryNumber = 0;
	typeCalvaryNumber = 0;
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
			if (dbUnit->typeInfantry) { ++typeInfantryNumber; }
			if (dbUnit->typeCalvary) { ++typeCalvaryNumber; }
			if (dbUnit->typeRange) { ++typeRangeNumber; }
			if (dbUnit->typeMelee) { ++typeMeleeNumber; }
			if (dbUnit->typeLight) { ++typeLightNumber; }
			if (dbUnit->typeHeavy) { ++typeHeavyNumber; }
			if (dbUnit->typeSpecial) { ++typeSpecialNumber; }
		}
	}


	idleArmyNumber = std::ranges::count_if(units, isFreeSolider);
	armyNumber = std::ranges::count_if(units, isSolider);

	resetSpan(metric->resWithoutBonus);
	for (const auto worker : workers) {
		if (worker->getState() == UnitState::COLLECT && worker->isFirstThingAlive()) {
			auto res = (ResourceEntity*)worker->getThingToInteract();

			const auto bonus = res->getBonus(worker->getPlayer());
			if (bonus <= 1.f) {
				metric->resWithoutBonus[res->getResourceId()] += 1;
			}
		}
	}

	assert(validateSpan(__LINE__, __FILE__, metric->unitsSum));
	assert(validateSpan(__LINE__, __FILE__, metric->freeArmySum));
	assert(validateSpan(__LINE__, __FILE__, metric->buildingsSum));
}
