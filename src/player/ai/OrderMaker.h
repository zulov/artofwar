#pragma once
#include <vector>

#include "nn/Brain.h"

enum class UnitState : char;
class Player;
class Unit;

class OrderMaker {
public:
	explicit OrderMaker(Player* player);
	OrderMaker(const OrderMaker& rhs) = delete;

	void action();
private:
	std::vector<Unit*> findFreeWorkers() const;
	void collect(std::vector<Unit*>& workers);

	Player* player;
	Brain collectResourceId;

};
