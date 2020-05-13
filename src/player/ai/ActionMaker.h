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
	std::optional<Urho3D::Vector2> posToBuild(db_building* building);
	std::vector<Building*> getBuildingsCanDeploy(db_unit* unit, std::vector<db_building*>& buildings) const;
	const std::vector<float>& inputWithParamsDecide(Brain& brain, const db_level* level) const;
	Building* getBuildingToDeploy(db_unit* unit);
	bool createBuilding();

	std::optional<short> chooseUpgrade(StatsOutputType order) const;
	db_building* chooseBuilding();
	int randFromThree(std::vector<float> diffs) const;
	float dist(std::valarray<float>& center, db_ai_property* props);

	db_unit* chooseUnit();

	Player* player;
	Brain mainBrain;
	
	Brain buildingBrainId;
	Brain buildingBrainPos;
	
	Brain unitBrainId;
	Brain unitBrainPos;
	
	Brain unitOrderId;
};
