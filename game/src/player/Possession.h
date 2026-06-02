#pragma once
#include <array>
#include <span>
#include <vector>
#include <objects/resource/ResourceType.h>


struct PossessionMetric;
enum class ParentBuildingType : char;
class Physical;
class Player;
class Unit;
class Building;
class Resources;

class Possession {
public:
	Possession(unsigned short nation);
	~Possession();
	Possession(const Possession&) = delete;

	void add(Building* building);
	void add(Unit* unit);
	void updateAndClean(Resources* resources);
	void ensureUnitMetrics();
	void ensureBuildingMetrics();
	void ensureCounts();
	void ensureResWithoutBonus();

	unsigned getScore() const;
	unsigned getUnitsNumber() const;
	unsigned getBuildingsNumber() const;
	unsigned getWorkersNumber() const;

	unsigned getFreeWorkersNumber();
	unsigned getFreeArmyNumber();
	unsigned getArmyNumber();
	unsigned getInfantryNumber();
	unsigned getCavalryNumber();
	unsigned getMeleeNumber();
	unsigned getRangeNumber();
	unsigned getLightNumber();
	unsigned getHeavyNumber();

	std::span<float> getResWithOutBonus();
	float getResWithOutBonus(ResourceType rt);
	std::vector<Building*>* getBuildings(short id);
	const std::vector<Building*>& getBuildings();

	void addKilled(Physical* physical);

	const PossessionMetric* getMetrics();

	const std::vector<Unit*>& getWorkers() const { return workers; }
	std::vector<Unit*> getFreeArmy();
	std::vector<Unit*> getAllArmy();
	bool hasAnyFreeArmy() const;
	float getAttackSum();
	float getFreeArmyAttackSum();
	float getDefenceAttackSum();

	unsigned getResourceBuildingCount();
	unsigned getConvertBuildingCount();
	unsigned getSpawnerCount();

	float getInCombatRatio();
	float getBonusCoverage(ResourceType rt);
	float getResWithoutBonusSum();
	float getValueDestroyed() const { return valueDestroyed; }
private:
	std::vector<Building*> buildings;
	std::vector<std::vector<Building*>*> buildingsPerId;

	std::vector<Unit*> units;
	std::vector<Unit*> workers;

	unsigned idleWorkersNumber = 0;
	unsigned idleArmyNumber = 0, armyNumber = 0;
	unsigned typeInfantryNumber = 0, typeCavalryNumber = 0;
	unsigned typeRangeNumber = 0, typeMeleeNumber = 0;
	unsigned typeHeavyNumber = 0, typeLightNumber = 0;
	unsigned typeSpecialNumber = 0;
	unsigned inCombatNumber = 0;

	unsigned resourceBuildingCount = 0;
	unsigned convertBuildingCount = 0;
	unsigned spawnerCount = 0;

	std::array<float, 4> bonusCoverage{};

	float resourcesSum = 0.f;
	float freeArmyAttackSum = 0.f;
	float valueDestroyed = 0.f;

	bool unitMetricsReady = false;
	bool buildingMetricsReady = false;
	bool countsReady = false;
	bool resWithoutBonusReady = false;

	int levelsSize;
	float* levels;

	PossessionMetric* metric;
};
