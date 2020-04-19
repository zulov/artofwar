#pragma once
#include <functional>
#include <optional>
#include <Urho3D/Math/Vector2.h>

#include "nn/Brain.h"

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
	void createBuilding();

	std::optional<short> chooseUpgrade(StatsOutputType order) const;
	void getValues(float* values, const std::function<float(db_level*)>& func) const;
	db_building* chooseBuilding();

	Player* player;
	Brain mainBrain;
	Brain buildingBrainId;
	Brain buildingBrainPos;
};
