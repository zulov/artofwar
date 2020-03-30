#pragma once
#include <optional>
#include <Urho3D/Math/Vector2.h>

class Player;
enum class StatsOutputType : char;

class ActionMaker {
public:
	explicit ActionMaker(Player* player);

	void createOrder(StatsOutputType order);
private:
	void upgrade(StatsOutputType order);
	void createUnit(StatsOutputType order);
	void createBuilding(StatsOutputType order);
	
	std::optional<short> chooseUpgrade(StatsOutputType order);
	short chooseBuilding(StatsOutputType order) const;
	Urho3D::Vector2 bestPosToBuild(StatsOutputType order, short id) const;
	Player* player;
};
