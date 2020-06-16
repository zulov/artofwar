#pragma once
#include <initializer_list>
#include <vector>

#include "nn/Brain.h"

enum class UnitState : char;
class Player;
class Unit;

class OrderMaker {
public:
	explicit OrderMaker(Player* player);

	void action();
private:
	bool isNotInStates(UnitState getState, std::initializer_list<UnitState> states);
	std::vector<Unit*> findFreeWorkers();
	void collect(std::vector<Unit*>& workers);

	Player* player;
	Brain collectResourceId;

};
