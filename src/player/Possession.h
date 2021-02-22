#pragma once
#include <magic_enum.hpp>
#include <span>
#include <vector>
#include "objects/Metrics.h"


class Player;
class ObjectsInfo;
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
	void updateAndClean(Resources& resources, const ObjectsInfo* simInfo);
	int getScore() const;
	int getUnitsNumber() const;
	int getBuildingsNumber() const;
	int getWorkersNumber() const;
	int getFreeWorkersNumber() const;
	std::vector<Building*>* getBuildings(short id);
	float getUnitsVal(UnitMetric value) const;
	float getBuildingsVal(BuildingMetric value) const;

	std::span<float> getUnitsMetrics() const { return unitsValuesAsSpan; }
	std::span<float> getFreeArmyMetrics() const { return freeArmyMetricsAsSpan; }
	std::span<float> getBuildingsMetrics() const { return buildingsValuesAsSpan; }

	const std::vector<Unit*>& getWorkers() const { return workers; }
	std::vector<Unit*> getFreeArmy();
private:
	std::vector<Building*> buildings;
	std::vector<std::vector<Building*>*> buildingsPerId;

	std::vector<Unit*> units;
	std::vector<Unit*> workers;
	int freeWorkersNumber = 0;
	float resourcesSum = 0;

	float data[magic_enum::enum_count<UnitMetric>() - 1
		+ magic_enum::enum_count<UnitMetric>() - 1
		+ magic_enum::enum_count<BuildingMetric>() - 1];

	const std::span<float> unitsValuesAsSpan = std::span(data, magic_enum::enum_count<UnitMetric>() - 1);
	const std::span<float> freeArmyMetricsAsSpan = std::span(data + magic_enum::enum_count<UnitMetric>() - 1,
	                                                         magic_enum::enum_count<UnitMetric>() - 1);
	const std::span<float> buildingsValuesAsSpan = std::span(
		data + (magic_enum::enum_count<UnitMetric>() - 1) * 2,
		magic_enum::enum_count<BuildingMetric>() - 1);
};
