#pragma once
#include <vector>

struct db_nation;
enum class UnitState : char;
class Player;
class Unit;
class Physical;
class Brain;
class Threshold;

class OrderMaker {
public:
	explicit OrderMaker(Player* player, db_nation* nation);
	OrderMaker(const OrderMaker& rhs) = delete;

	void action();
private:
	std::vector<Unit*> findFreeWorkers() const;
	
	Physical * closetInRange(Unit* worker, int resourceId, float radius);
	void collect(std::vector<Unit*>& workers);

	Player* player;
	Brain *whichResource;
	Threshold* attackThreshold;
};
