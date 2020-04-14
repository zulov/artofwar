#pragma once
#include <functional>
#include <optional>
#include <Urho3D/Math/Vector2.h>


#include "database/db_strcut.h"
#include "nn/Brain.h"
#include "objects/building/Building.h"

class Player;
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
	void createBuilding();

	std::optional<short> chooseUpgrade(StatsOutputType order) const;
	void getValues(float* values, const std::function<float(db_ai_prop_level*)>& func) const;
	short chooseBuilding();

	Player* player;
	Brain mainBrain;
	Brain buildingBrain;
};
