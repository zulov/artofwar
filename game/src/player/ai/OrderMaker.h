#pragma once
#include <vector>

#include "env/influence/CenterType.h"
#include "utils/defines.h"

class AiInputProvider;

namespace Urho3D {
	class Vector2;
}

class Possession;
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
	std::vector<Physical*>* getThingsToAttack(CenterType centerType, Unit* unit);
	void actCollect(char resId, std::vector<Unit*>& rest, std::vector<Unit*>& workers);
	std::vector<Unit*> getSubGroup(std::vector<std::vector<Unit*>>& groups, unsigned char n);
	void collect(std::vector<Unit*>& freeWorkers);

	Player* player;
	char playerId;
	Possession* possession;
	AiInputProvider* aiInput;

	Brain* whichResource;

	Brain* attackOrDefence;
	Brain* whereAttack;
	Brain* whereDefence;

	std::vector<Physical*>* temp;
	unsigned char resHistogram[RESOURCES_SIZE];

};
