#include "Possession.h"
#include "Game.h"
#include "Resources.h"
#include "ai/AiMetric.h"
#include "database/DatabaseCache.h"
#include "math/SpanUtils.h"
#include "math/VectorUtils.h"
#include "objects/PhysicalUtils.h"
#include "objects/building/Building.h"
#include "objects/building/ParentBuildingType.h"

Possession::Possession(char nation) {
	for (auto building : Game::getDatabase()->getNation(nation)->buildings) {
		auto id = building->id;
		if (buildingsPerId.size() <= id) {
			buildingsPerId.resize(id + 1, nullptr);
		}
		buildingsPerId[id] = new std::vector<Building*>();
	}
	int dataSize = UNIT_SIZE * 2 + BUILDING_SIZE
		+ BUILDING_OTHER_SIZE + BUILDING_DEF_SIZE
		+ BUILDING_TECH_SIZE
		+ BUILDING_UNITS_SIZE + RESOURCES_SIZE;
	data = new float[dataSize];

	int begin = 0;
	unitsSumAsSpan = std::span(data, UNIT_SIZE);
	freeArmySumAsSpan = std::span(data + (begin += UNIT_SIZE), UNIT_SIZE);
	buildingsSumAsSpan = std::span(data + (begin += UNIT_SIZE), BUILDING_SIZE);

	buildingsOtherSumSpan = std::span(data + (begin += BUILDING_SIZE), BUILDING_OTHER_SIZE);
	buildingsDefenceSumSpan = std::span(data + (begin += BUILDING_OTHER_SIZE), BUILDING_DEF_SIZE);

	buildingsTechSumSpan = std::span(data + (begin += BUILDING_DEF_SIZE), BUILDING_TECH_SIZE);
	buildingsUnitsSumSpan = std::span(data + (begin += BUILDING_TECH_SIZE), BUILDING_UNITS_SIZE);
	resWithoutBonus = std::span(data + (begin += BUILDING_UNITS_SIZE), RESOURCES_SIZE);

	std::fill_n(data, dataSize, 0.f);

	levelsSize = Urho3D::Max(Game::getDatabase()->getUnitLevels().size(),
	                         Game::getDatabase()->getBuildingLevels().size());

	levels = new float[levelsSize];
	levelsFree = new float[levelsSize];
	std::fill_n(levels, levelsSize, 0.f);
	std::fill_n(levelsFree, levelsSize, 0.f);
}

Possession::~Possession() {
	clear_vector(buildingsPerId);
	delete[]data;
	delete[]levels;
	delete[]levelsFree;
}

int Possession::getScore() const {
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
		if (unit->getDbUnit()->typeWorker) {
			workerScore += unit->getCostSum();
		} else {
			unitsScore += unit->getCostSum();
		}
	}

	return buildingScore * 0.25f + unitsScore * 0.2f + workerScore * 0.05f + resourcesSum * 0.01f;
	//return buildings.size() * 10 + units.size() + resourcesSum / 100;
}

int Possession::getUnitsNumber() const {
	return units.size();
}

int Possession::getBuildingsNumber() const {
	return buildings.size();
}

int Possession::getWorkersNumber() const {
	return workers.size();
}

int Possession::getFreeWorkersNumber() {
	ensureReady();
	return idleWorkersNumber;
}

int Possession::getFreeArmyNumber() {
	ensureReady();
	return idleArmyNumber;
}

int Possession::getArmyNumber() {
	ensureReady();
	return armyNumber;
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

std::span<float> Possession::refreshBuildingSum(const std::span<unsigned char> idxs, std::span<float> out) const {
	assert(idxs.size() == out.size());
	for (int i = 0; i < idxs.size(); ++i) {
		out[i] = buildingsSumAsSpan[idxs[i]];
	}
	return out;
}

std::span<float> Possession::getBuildingsMetrics(ParentBuildingType type) {
	ensureReady();
	switch (type) {
	case ParentBuildingType::OTHER:
		return refreshBuildingSum(METRIC_DEFINITIONS.getBuildingOtherIdxs(), buildingsOtherSumSpan);
	case ParentBuildingType::DEFENCE:
		return refreshBuildingSum(METRIC_DEFINITIONS.getBuildingDefenceIdxs(), buildingsDefenceSumSpan);
	case ParentBuildingType::RESOURCE:
		assert(false);
	//return refreshResource(METRIC_DEFINITIONS.getResWithoutBonusIdxs(), buildingsResSumSpan);
	case ParentBuildingType::TECH:
		return refreshBuildingSum(METRIC_DEFINITIONS.getBuildingTechIdxs(), buildingsTechSumSpan);
	case ParentBuildingType::UNITS:
		return refreshBuildingSum(METRIC_DEFINITIONS.getBuildingUnitsIdxs(), buildingsUnitsSumSpan);
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
	return unitsSumAsSpan[5];
	//TODO hardcoded from AiUnitMetric {[](db_unit* u, db_unit_level* l) -> float { return l->attack; }, 10, UNITS_SUM_X},
}

float Possession::getDefenceAttackSum() {
	ensureReady();
	return buildingsSumAsSpan[5];

	//TODO hardcoded from AiBuildingMetric {[](db_building* b, db_building_level* l) -> float { return l->attack; }, 20, BUILDINGS_SUM_X},
}

void Possession::add(Building* building) {
	buildings.push_back(building);
	buildingsPerId[building->getId()]->push_back(building);
}

void Possession::add(Unit* unit) {
	units.push_back(unit);

	if (unit->getDbUnit()->typeWorker) {
		workers.push_back(unit);
	}
}

void Possession::updateAndClean(const Resources& resources) {
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
	resourcesSum = sumSpan(resources.getValues());
}

void Possession::ensureReady() {
	if (ready) { return; }
	ready = true;
	resetSpan(unitsSumAsSpan);
	resetSpan(freeArmySumAsSpan);

	assert(zerosSpan(levels, levelsSize));
	assert(zerosSpan(levelsFree, levelsSize));

	for (const auto unit : units) {
		auto per = unit->getHealthPercent();
		levels[unit->getLevel()->id] += per;

		//TODO to dac jako któtkie
		if (isFreeSolider(unit)) {
			levelsFree[unit->getLevel()->id] += per;
		}
	}
	auto& uLevels = Game::getDatabase()->getUnitLevels();
	for (int i = 0; i < levelsSize; ++i) {
		const auto val = levels[i];
		if (val > 0.f) {
			auto& metric = uLevels[i]->dbUnitMetric->getValuesNormForSum();
			assert(metric.size() == unitsSumAsSpan.size());
			for (int j = 0; j < unitsSumAsSpan.size(); ++j) {
				unitsSumAsSpan[j] += val * metric[j];
			}
			const auto val2 = levelsFree[i];
			if (val2 > 0.f) {
				for (int j = 0; j < freeArmySumAsSpan.size(); ++j) {
					freeArmySumAsSpan[j] += val2 * metric[j];
				}
				levelsFree[i] = 0.f;
			}
			levels[i] = 0.f;
		}
	}

	resetSpan(buildingsSumAsSpan);

	zerosSpan(levels, levelsSize);
	zerosSpan(levelsFree, levelsSize);

	for (const auto building : buildings) {
		levels[building->getLevel()->id] += building->getHealthPercent();
	}

	auto& bLevels = Game::getDatabase()->getBuildingLevels();
	for (int i = 0; i < levelsSize; ++i) {
		const auto lVal = levels[i];
		if (lVal > 0.f) {
			auto& metric = bLevels[i]->dbBuildingMetric->getValuesNormForSum();
			assert(metric.size() == buildingsSumAsSpan.size());
			for (int j = 0; j < buildingsSumAsSpan.size(); ++j) {
				buildingsSumAsSpan[j] += lVal * metric[j];
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
			const auto dbUnit = unit->getDbUnit();
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

	resetSpan(resWithoutBonus);
	for (const auto worker : workers) {
		if (worker->getState() == UnitState::COLLECT && worker->isFirstThingAlive()) {
			auto res = (ResourceEntity*)worker->getThingToInteract();

			const auto bonus = res->getBonus(worker->getPlayer());
			if (bonus <= 1.f) {
				resWithoutBonus[res->getResourceId()] += 1;
			}
		}
	}

	assert(validateSpan(__LINE__, __FILE__, unitsSumAsSpan));
	assert(validateSpan(__LINE__, __FILE__, freeArmySumAsSpan));
	assert(validateSpan(__LINE__, __FILE__, buildingsSumAsSpan));
}
