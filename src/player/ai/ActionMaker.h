#pragma once
#include <functional>
#include <optional>
#include <Urho3D/Math/Vector2.h>
#include "nn/Brain.h"

class Player;
struct db_level;
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
	Urho3D::Vector2 posToBuild(short idToCreate);
	void createBuilding();

	std::optional<short> chooseUpgrade(StatsOutputType order) const;
	void getValues(float* values, const std::function<float(db_level*)>& func) const;
	short chooseBuilding();

	Player* player;
	Brain mainBrain;
	Brain buildingBrainId;
	Brain buildingBrainPos;
};
