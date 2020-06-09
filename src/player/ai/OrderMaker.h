#pragma once
#include "nn/Brain.h"
#include "objects/unit/Unit.h"

class Player;

class OrderMaker {
public:
	explicit OrderMaker(Player* player);

	void action();
private:
	bool isNotInStates(UnitState getState, std::initializer_list<UnitState> states);
	std::vector<Unit*> findFreeWorkers();
	bool collect(std::vector<Unit*>& workers);

	Player* player;
	Brain collectResourceId;

};
