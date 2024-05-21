#pragma once
#include <span>
#include <vector>

enum class ParentBuildingType : char;
class Physical;
class Player;
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
	void updateAndClean(const Resources& resources);
	void ensureReady();

	unsigned getScore() const;
	unsigned getUnitsNumber() const;
	unsigned getBuildingsNumber() const;
	unsigned getWorkersNumber() const;

	unsigned getFreeWorkersNumber();
	unsigned getFreeArmyNumber();
	unsigned getArmyNumber();
	unsigned getInfantryNumber();
	unsigned getCalvaryNumber();
	unsigned getMeleeNumber();
	unsigned getRangeNumber();
	unsigned getLightNumber();
	unsigned getHeavyNumber();

	std::span<float> getResWithOutBonus() { ensureReady(); return resWithoutBonus; }
	std::vector<Building*>* getBuildings(short id);
	const std::vector<Building*>& getBuildings();

	void addKilled(Physical* physical);

	std::span<float> getUnitsMetrics() { ensureReady(); return unitsSumAsSpan; }
	std::span<float> getFreeArmyMetrics() { ensureReady(); return freeArmySumAsSpan; }
	std::span<float> getBuildingsMetrics() { ensureReady(); return buildingsSumAsSpan; }
	std::span<float> getBuildingsMetrics(ParentBuildingType type);

	const std::vector<Unit*>& getWorkers() const { return workers; }
	std::vector<Unit*> getFreeArmy();
	bool hasAnyFreeArmy() const;
	float getAttackSum();
	float getDefenceAttackSum();
private:
	std::span<float> refreshBuildingSum(const std::span<unsigned char> idxs, std::span<float> out) const;
	std::vector<Building*> buildings;
	std::vector<std::vector<Building*>*> buildingsPerId;

	std::vector<Unit*> units;
	std::vector<Unit*> workers;

	unsigned idleWorkersNumber = 0;
	unsigned idleArmyNumber = 0, armyNumber = 0;
	unsigned typeInfantryNumber = 0, typeCalvaryNumber = 0;
	unsigned typeRangeNumber = 0, typeMeleeNumber = 0;
	unsigned typeHeavyNumber = 0, typeLightNumber = 0;
	unsigned typeSpecialNumber = 0;

	float resourcesSum = 0.f;
	float resourcesDestroyed = 0.f;

	bool ready = false;

	int levelsSize;
	float* levels;
	float* levelsFree;

	float* data;

	std::span<float> unitsSumAsSpan;//zrobic z tego array
	std::span<float> freeArmySumAsSpan; //TODO to dac jako któtkie
	std::span<float> buildingsSumAsSpan;

	std::span<float> buildingsOtherSumSpan;
	std::span<float> buildingsDefenceSumSpan;
	std::span<float> resWithoutBonus;
	std::span<float> buildingsTechSumSpan;
	std::span<float> buildingsUnitsSumSpan;

};
