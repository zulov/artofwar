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
private:
	float* decide(Brain& brain) const;
	void createOrder(StatsOutputType order);
	void levelUpUnit();
	void levelUpBuilding();
	void createUnit();
	std::optional<Urho3D::Vector2> posToBuild(db_building* building);
	Building* getBuildingToDeploy(db_unit* unit) const;
	void createBuilding();

	std::optional<short> chooseUpgrade(StatsOutputType order) const;
	db_building* chooseBuilding();
	void closest(std::valarray<float> &center, short& closestId, float& closest, db_ai_property* props, short id);
	db_unit* chooseUnit();

	Player* player;
	Brain mainBrain;
	
	Brain buildingBrainId;
	Brain buildingBrainPos;
	
	Brain unitBrainId;
	Brain unitBrainPos;
};
