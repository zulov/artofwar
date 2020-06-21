#include "OrderMaker.h"

#include "ActionCenter.h"
#include "AiUtils.h"
#include "Game.h"
#include "commands/action/UnitActionCommand.h"
#include "objects/PhysicalUtils.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/IndividualOrder.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "player/Player.h"
#include "simulation/env/Environment.h"
#include "stats/Stats.h"

OrderMaker::OrderMaker(Player* player)
	: player(player), collectResourceId("Data/ai/resourceId_w.csv") {
}

void OrderMaker::action() {
	auto freeWorkers = findFreeWorkers();

	if (!freeWorkers.empty()) {
		collect(freeWorkers);
	}
}

bool OrderMaker::isNotInStates(UnitState state, std::initializer_list<UnitState> states) {
	for (auto s : states) {
		if (state == s) {
			return false;
		}
	}
	return true;
}

std::vector<Unit*> OrderMaker::findFreeWorkers() {
	std::vector<Unit*> freeWorkers;
	for (auto worker : player->getPossession().getWorkers()) {
		if (isNotInStates(worker->getState(), {UnitState::COLLECT, UnitState::GO_TO})) {
			freeWorkers.push_back(worker);
		}
	}
	return freeWorkers;
}

void OrderMaker::collect(std::vector<Unit*>& workers) {
	auto input = Game::getStats()->getResourceIdInput(player->getId());
	auto result = collectResourceId.decide(input);
	auto resourceId = biggestWithRand(result);
	for (auto worker : workers) {
		auto pos = worker->getPosition();
		auto list = Game::getEnvironment()->getResources(worker->getPosition(), 128, resourceId);
		auto [closest, minDistance, indexToInteract] = worker->closestPhysical(list, belowClose);
		//TODO zgrupowaæ i uwzglêdniæ limit
		if (closest) {
			Game::getActionCenter()->add(
				new UnitActionCommand(new IndividualOrder(worker, UnitAction::COLLECT, closest, false),
				                      player->getId()));
		}

	}
}
