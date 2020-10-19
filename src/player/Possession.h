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
	void updateAndClean(Resources& resources, SimulationInfo* simInfo);
	int getScore() const;
	int getUnitsNumber() const;
	int getBuildingsNumber() const;
	int getWorkersNumber() const;
	int getFreeWorkersNumber() const;
	std::vector<Building*>* getBuildings(short id);
	float getUnitsVal(UnitMetric value) const;
	float getBuildingsVal(BuildingMetric value) const;

	std::span<float> getUnitsMetrics() const;
	std::span<float> getBuildingsMetrics() const;

	std::vector<Unit*>& getWorkers();
private:
	std::vector<Building*> buildings;
	std::vector<std::vector<Building*>*> buildingsPerId;

	std::vector<Unit*> units;
	std::vector<Unit*> workers;
	int freeWorkersNumber = 0;
	float resourcesSum = 0;

	float unitsMetrics[magic_enum::enum_count<UnitMetric>()-1];
	float buildingsMetrics[magic_enum::enum_count<BuildingMetric>()-1];
	const std::span<float> unitsValuesAsSpan = std::span(unitsMetrics);
	const std::span<float> buildingsValuesAsSpan = std::span(buildingsMetrics);
};
