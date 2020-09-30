#pragma once
#include <magic_enum.hpp>
#include <span>
#include <vector>

#include "objects/Metrics.h"


class SimulationInfo;
class Unit;
class Building;
class Resources;

class Possession {
public:
	Possession(char nation);
	~Possession();
	Possession(const Possession&) = delete;
	void add(Building* building);
	void add(Unit* unit);
	void updateAndClean(Resources& resources, SimulationInfo * simInfo);
	int getScore() const;
	int getUnitsNumber() const;
	int getWorkersNumber() const;
	int getAttackScore() const;
	int getDefenceScore() const;
	std::vector<Building*>* getBuildings(short id);
	float getUnitsVal(UnitMetric value) const;
	float getBuildingsVal(BuildingMetric value) const;
	std::vector<Unit*>& getWorkers();
private:
	std::vector<Building*> buildings;
	std::vector<std::vector<Building*>*> buildingsPerId;

	std::vector<Unit*> units;
	std::vector<Unit*> workers;
	float resourcesSum = 0;
	float attackSum = 0;
	float defenceSum = 0;

	float unitsMetrics[magic_enum::enum_count<UnitMetric>()];
	float buildingsMetrics[magic_enum::enum_count<BuildingMetric>()];
	std::span<float> unitsValuesAsSpan;
	std::span<float> buildingsValuesAsSpan;
	
};
