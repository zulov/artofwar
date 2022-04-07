#pragma once
#include <vector>

struct db_nation;
enum class UnitState : char;
class Player;
class Unit;
class Physical;
class Brain;

class OrderMaker {
public:
	explicit OrderMaker(Player* player, db_nation* nation);
	void semiCloseAttack(const std::vector<Unit*>& subArmy, const std::vector<Physical*>& things) const;
	OrderMaker(const OrderMaker& rhs) = delete;

	void action();
private:
	std::vector<Unit*> findFreeWorkers() const;

	Physical* closetInRange(Unit* worker, int resourceId);
	void actCollect(unsigned char& resHistogram, char resId, std::vector<Unit*>& rest, std::vector<Unit*>& workers);
	void collect(std::vector<Unit*>& freeWorkers);

	Player* player;

	Brain* whichResource;

	Brain* attackOrDefence;
	Brain* whereAttack;
	Brain* whereDefence;

};
