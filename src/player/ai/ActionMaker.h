#pragma once
#include <optional>
#include <valarray>
#include <Urho3D/Math/Vector2.h>
#include "nn/Brain.h"


struct db_basic_metric;
class Player;
class Building;
struct db_with_cost;
struct db_level;
struct db_building;
struct db_unit_level;
struct db_unit;
struct db_building_level;

class ActionMaker {
public:
	explicit ActionMaker(Player* player);
	ActionMaker(const ActionMaker& rhs) = delete;
	void action();
private:
	bool createUnit(db_unit* unit, Building* building) const;
	bool createUnit(db_unit* unit);
	bool createWorker(db_unit* unit);

	bool enoughResources(db_with_cost* withCosts) const;

	bool levelUpUnit();
	bool levelUpBuilding();
	bool createBuilding(db_building* building);

	std::optional<Urho3D::Vector2> posToBuild(db_building* building);
	std::vector<Building*> getBuildingsCanDeploy(short unitId) const;

	float dist(std::valarray<float>& center, const db_basic_metric* metric);

	db_building* chooseBuilding(std::span<float> result);
	db_building_level* chooseBuildingLevelUp();
	db_unit* chooseUnit(std::span<float> result);
	db_unit_level* chooseUnitLevelUp();

	Building* getBuildingToDeploy(db_unit* unit);
	Building* getBuildingToDeployWorker(db_unit* unit);
	Building* getBuildingToLevelUpUnit(db_unit_level* level);
	Building* getBuildingClosestArea(std::vector<Building*>& allPossible, std::span<float> result) const;

	Player* player;

	Brain ifWorkersCreate;
	Brain whereWorkersCreate;

	Brain ifBuildingCreate;
	Brain whichBuildingCreate;
	Brain whereBuildingCreate;

	Brain ifUnitCreate;
	Brain whichUnitCreate;
	Brain whereUnitCreate;

};
