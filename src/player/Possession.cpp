#include "Possession.h"
#include "Game.h"
#include "Resources.h"
#include "ai/AiMetric.h"
#include "database/DatabaseCache.h"
#include "math/SpanUtils.h"
#include "math/VectorUtils.h"
#include "objects/PhysicalUtils.h"
#include "objects/building/Building.h"
#include "simulation/ObjectsInfo.h"

Possession::Possession(char nation) {
	for (auto building : Game::getDatabase()->getNation(nation)->buildings) {
		auto id = building->id;
		if (buildingsPerId.size() <= id) {
			buildingsPerId.resize(id + 1, nullptr);
		}
		buildingsPerId[id] = new std::vector<Building*>();
	}
	char unitSize = std::size(METRIC_DEFINITIONS.aiUnitMetric);
	char buildingSize = std::size(METRIC_DEFINITIONS.aiBuildingMetric);

	data = new float[unitSize * 2 + buildingSize];

	unitsSumAsSpan = std::span<float>(data, unitSize);
	freeArmySumAsSpan = std::span<float>(data + unitSize, unitSize);
	buildingsSumAsSpan = std::span<float>(data + 2 * unitSize, buildingSize);

	resetSpan(unitsSumAsSpan);
	resetSpan(freeArmySumAsSpan);
	resetSpan(buildingsSumAsSpan);
}

Possession::~Possession() {
	clear_vector(buildingsPerId);
	delete[]data;
}


int Possession::getScore() const {
	float buildingScore = 0.f;
	for (auto building : buildings) {
		auto cost = building->getCostSum();

		if (!building->isReady()) {
			cost *= 0.5;
		}
		buildingScore += cost;
	}

	float unitsScore = 0.f;
	float workerScore = 0.f;
	for (auto unit : units) {
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

int Possession::getFreeWorkersNumber() const { return freeWorkersNumber; }

std::vector<Building*>* Possession::getBuildings(short id) {
	return buildingsPerId[id];
}

void Possession::addKilled(Physical* physical) {
	resourcesDestroyed += physical->getCostSum();
}

std::vector<Unit*> Possession::getFreeArmy() {
	std::vector<Unit*> army(units.size());

	auto it = std::ranges::copy_if(units, army.begin(), [](Unit* unit) {
		return isFreeSolider(unit);
	}).out;
	army.resize(std::distance(army.begin(), it));
	return army;
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

void Possession::updateAndClean(const Resources& resources, const ObjectsInfo* simInfo) {
	cleanDead(buildings, simInfo->ifBuildingDied());
	cleanDead(units, simInfo->ifUnitDied());
	cleanDead(workers, simInfo->ifUnitDied());

	if (simInfo->ifBuildingDied()) {
		for (const auto perId : buildingsPerId) {
			if (perId) {
				cleanDead(perId);
			}
		}
	}
	resetSpan(unitsSumAsSpan);
	resetSpan(freeArmySumAsSpan);

	//TODO perf zliczyc levele i tak to zsumowac
	for (const auto unit : units) {
		unit->addValues(unitsSumAsSpan);

		if (isFreeSolider(unit)) {
			unit->addValues(freeArmySumAsSpan);
		}
	}
	resetSpan(buildingsSumAsSpan);
	for (const auto building : buildings) {
		building->addValues(buildingsSumAsSpan);
	}

	resourcesSum = sumSpan(resources.getValues());

	freeWorkersNumber = std::ranges::count_if(workers, [](Unit* worker) {
		return isInFreeState(worker->getState());
	});

	assert(validateSpan(__LINE__, __FILE__, unitsSumAsSpan));
	assert(validateSpan(__LINE__, __FILE__, freeArmySumAsSpan));
	assert(validateSpan(__LINE__, __FILE__, buildingsSumAsSpan));
}
