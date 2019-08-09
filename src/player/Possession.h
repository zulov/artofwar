#pragma once
#include <vector>


class Unit;
class Building;
class Resources;

class Possession {
public:
	int getScore();
	void add(Building* building);
	void add(Unit* unit);
	void updateAndClean(Resources& resources);
private:
	std::vector<Building*> buildings;
	std::vector<Unit*> units;
	float resourcesSum;
};
