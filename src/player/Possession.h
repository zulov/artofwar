#pragma once
#include <span>
#include <vector>


class Physical;
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
	void updateAndClean(const Resources& resources, const ObjectsInfo* simInfo);
	int getScore() const;
	int getUnitsNumber() const;
	int getBuildingsNumber() const;
	int getWorkersNumber() const;
	int getFreeWorkersNumber() const;
	std::vector<Building*>* getBuildings(short id);

	void addKilled(Physical* physical);

	std::span<float> getUnitsMetrics() const { return unitsSumAsSpan; }
	std::span<float> getFreeArmyMetrics() const { return freeArmySumAsSpan; }
	std::span<float> getBuildingsMetrics() const { return buildingsSumAsSpan; }

	const std::vector<Unit*>& getWorkers() const { return workers; }
	std::vector<Unit*> getFreeArmy();
private:
	std::vector<Building*> buildings;
	std::vector<std::vector<Building*>*> buildingsPerId;

	std::vector<Unit*> units;
	std::vector<Unit*> workers;
	int freeWorkersNumber = 0;
	float resourcesSum = 0.f;
	float resourcesDestroyed = 0.f;

	float *data;
	
	std::span<float> unitsSumAsSpan;
	std::span<float> freeArmySumAsSpan;
	std::span<float> buildingsSumAsSpan;
};
