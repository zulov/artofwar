#pragma once
#include <functional>
#include <optional>
#include <Urho3D/Math/Vector2.h>


#include "database/db_strcut.h"
#include "objects/building/Building.h"

class Player;
enum class StatsOutputType : char;

class ActionMaker {
public:
	explicit ActionMaker(Player* player);

	void createOrder(StatsOutputType order);
private:
	void levelUpUnit();
	void levelUpBuilding();
	void createUnit(StatsOutputType order);
	void createBuilding(StatsOutputType order) const;
	
	std::optional<short> chooseUpgrade(StatsOutputType order) const;
	void getValues(float* values, const std::function<float(db_ai_prop_level*)>& func) const;
	short chooseBuilding(StatsOutputType order) const;
	Urho3D::Vector2 bestPosToBuild(StatsOutputType order, short id) const;
	Player* player;
};
