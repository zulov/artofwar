#pragma once
#include <magic_enum.hpp>
#include <vector>

#include "objects/ValueType.h"


class Unit;
class Building;
class Resources;

class Possession {
public:
	Possession(char nation);
	~Possession();
	
	void add(Building* building);
	void add(Unit* unit);
	void updateAndClean(Resources& resources);
	int getScore() const;
	int getUnitsNumber() const;
	int getWorkersNumber() const;
	int getAttackScore() const;
	int getDefenceScore() const;
	std::vector<Building*>* getBuildings(short id);
	float getUnitsVal(ValueType value) const;
	float getBuildingsVal(ValueType value) const;
private:
	std::vector<Building*> buildings;
	std::vector<std::vector<Building*>*> buildingsPerId;

	std::vector<Unit*> units;
	std::vector<Unit*> workers;
	float resourcesSum = 0;
	float attackSum = 0;
	float defenceSum = 0;

	float unitsValues[magic_enum::enum_count<ValueType>()]; 
	float buildingsValues[magic_enum::enum_count<ValueType>()]; 
};
