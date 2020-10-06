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
enum class StatsOutputType : char;

class ActionMaker {
public:
	explicit ActionMaker(Player* player);
	ActionMaker(const ActionMaker& rhs) = delete;
	void action();
private:
	const std::span<float> decideFromBasic(Brain& brain) const;
	bool createUnit(db_unit* unit);

	bool enoughResources(db_with_cost* withCosts) const;
	bool createOrder(StatsOutputType order);
	
	bool levelUpUnit();
	bool levelUpBuilding();
	bool createUnit();
	bool createBuilding();
	
	std::optional<Urho3D::Vector2> posToBuild(db_building* building);
	std::vector<Building*> getBuildingsCanDeploy(short unitId, std::vector<db_building*>& buildings) const;
	const std::span<float> inputWithParamsDecide(Brain& brain, const db_basic_metric* metric) const;

	float dist(std::valarray<float>& center, const db_basic_metric* metric);

	db_building* chooseBuilding();
	db_building_level* chooseBuildingLevelUp();
	db_unit* chooseUnit();
	db_unit_level* chooseUnitLevelUp();

	Building* getBuildingToDeploy(db_unit* unit);
	Building* getBuildingToLevelUpUnit(db_unit_level* level);

	Player* player;
	//Brain mainBrain;

	Brain buildingBrainId;
	Brain buildingBrainPos;

	Brain unitBrainId;
	Brain unitBrainPos;

	Brain unitOrderId;

	Brain buildingLevelUpId;
	Brain unitLevelUpId;
	Brain unitLevelUpPos;

	Brain ifWorkerNeeded;
};
