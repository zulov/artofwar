#pragma once
#include <vector>

#include "nn/Brain.h"
#include "threshold/ThresholdCalculator.h"

enum class UnitState : char;
class Player;
class Unit;
class Physical;

class OrderMaker {
public:
	explicit OrderMaker(Player* player);
	OrderMaker(const OrderMaker& rhs) = delete;

	void action();
private:
	std::vector<Unit*> findFreeWorkers() const;
	
	Physical * closetInRange(Unit* worker, int resourceId, float radius);
	void collect(std::vector<Unit*>& workers);

	Player* player;
	Brain whatResource;
	ThresholdCalculator threshold;
	
};
