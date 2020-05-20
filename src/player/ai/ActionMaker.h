#pragma once
#include <optional>
#include <valarray>
#include <Urho3D/Math/Vector2.h>
#include "database/db_strcut.h"
#include "nn/Brain.h"
#include "objects/building/Building.h"

class Player;
struct db_level;
struct db_building;
enum class StatsOutputType : char;

class ActionMaker {
public:
	explicit ActionMaker(Player* player);

	void action();
	bool enoughResources(db_with_cost* withCosts) const;
private:
	const std::vector<float>& decide(Brain& brain) const;
	bool createOrder(StatsOutputType order);
	
	bool levelUpUnit();
	bool levelUpBuilding();
	bool createUnit();
	bool createBuilding();
	
	std::optional<Urho3D::Vector2> posToBuild(db_building* building);
	std::vector<Building*> getBuildingsCanDeploy(short unitId, std::vector<db_building*>& buildings) const;
	const std::vector<float>& inputWithParamsDecide(Brain& brain, const db_ai_property* ai_property) const;

	float dist(std::valarray<float>& center, db_ai_property* props);

	db_building* chooseBuilding();
	db_building_level* chooseBuildingLevelUp();
	db_unit* chooseUnit();
	db_unit_level* chooseUnitLevelUp();

	Building* getBuildingToDeploy(db_unit* unit);
	Building* getBuildingToLevelUpUnit(db_unit_level* level);

	Player* player;
	Brain mainBrain;

	Brain buildingBrainId;
	Brain buildingBrainPos;

	Brain unitBrainId;
	Brain unitBrainPos;

	Brain unitOrderId;

	Brain buildingLevelUpId;
	Brain unitLevelUpId;
	Brain unitLevelUpPos;
};
