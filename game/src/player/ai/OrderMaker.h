#pragma once
#include <vector>
#include <Urho3D/Math/Vector2.h>

class UnitOrder;
struct db_nation;
enum class UnitState : char;
class Player;
class Unit;
class Physical;
class Brain;

class OrderMaker {
public:
	explicit OrderMaker(Player* player, db_nation* nation);
	~OrderMaker();
	OrderMaker(const OrderMaker& rhs) = delete;

	void action();
private:
	std::vector<Unit*> findFreeWorkers() const;
	void semiCloseAttack(const std::vector<Unit*>& subArmy, const std::vector<Physical*>* things) const;
	Physical* closetInRange(Unit* worker, int resourceId);
	UnitOrder* unitOrderGo(std::vector<Unit*>& subArmy, Urho3D::Vector2& center) const;
	UnitOrder* unitOrderCollect(std::vector<Unit*>& workers, Physical* closest) const;
	void actCollect(unsigned char& resHistogram, char resId, std::vector<Unit*>& rest, std::vector<Unit*>& workers);
	void collect(std::vector<Unit*>& freeWorkers);

	Player* player;

	Brain* whichResource;

	Brain* attackOrDefence;
	Brain* whereAttack;
	Brain* whereDefence;

	std::vector<Physical*>* temp;

};
