#pragma once
#include <vector>


class Unit;
class Building;
class Resources;

class Possession {
public:
	void add(Building* building);
	void add(Unit* unit);
	void updateAndClean(Resources& resources);
	int getScore() const;
	int getUnitsNumber() const;
	int getBuildingsNumber() const;
	int getWorkersNumber() const;
	int getAttackScore() const;
	int getDefenceScore() const;
private:
	std::vector<Building*> buildings;
	std::vector<Unit*> units;
	std::vector<Unit*> workers;
	float resourcesSum;
	float attackSum;
	float defenceSum;
};
