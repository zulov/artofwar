#include "OrderMaker.h"

#include "ActionCenter.h"
#include "AiUtils.h"
#include "Game.h"
#include "objects/PhysicalUtils.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/IndividualOrder.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/env/Environment.h"
#include "stats/AiInputProvider.h"
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "objects/unit/order/GroupOrder.h"
#include "objects/unit/order/enums/UnitActionType.h"
#include "threshold/Threshold.h"
#include "threshold/ThresholdProvider.h"

OrderMaker::OrderMaker(Player* player)
	: player(player), whatResource(BrainProvider::get("whichResource_w.csv")),
	  threshold(ThresholdProvider::get("attack_t.csv")) {
}

void OrderMaker::action() {
	auto freeWorkers = findFreeWorkers();

	if (!freeWorkers.empty()) {
		collect(freeWorkers);
	}
	auto& possesion = player->getPossession();

	bool ifAttack = threshold->ifDo(possesion.getFreeArmyMetrics());
	if (ifAttack) {
		char id = threshold->getBest(possesion.getFreeArmyMetrics());

		const char enemy = Game::getPlayersMan()->getEnemyFor(player->getId());
		Urho3D::Vector2 pos = Game::getEnvironment()->getCenterOf(CenterType(id), enemy);

		std::vector<Unit*> army = possesion.getFreeArmy();
		if (!army.empty()) {
			Game::getActionCenter()->addUnitAction(
				new GroupOrder(army, UnitActionType::ORDER, cast(UnitAction::GO), pos), player->getId());
		}
	}
}

std::vector<Unit*> OrderMaker::findFreeWorkers() const {
	std::vector<Unit*> freeWorkers;
	for (auto worker : player->getPossession().getWorkers()) {
		if (isFreeWorker(worker)) {
			freeWorkers.push_back(worker);
		}
	}
	return freeWorkers;
}

Physical* OrderMaker::closetInRange(Unit* worker, int resourceId, float radius) {
	auto list = Game::getEnvironment()->getResources(worker->getPosition(), radius, resourceId);
	auto [closest, minDistance, indexToInteract] = worker->closestPhysical(list, belowClose);
	return closest;
}

void OrderMaker::collect(std::vector<Unit*>& workers) {
	auto input = Game::getAiInputProvider()->getResourceInput(player->getId());
	auto result = whatResource->decide(input);

	for (auto worker : workers) {
		const auto resourceId = biggestWithRand(result);

		for (auto radius : {64.f, 128.f, 256.f}) {
			//TODO bug cos nei uwzglednia tej odleglsoci
			auto closest = closetInRange(worker, resourceId, radius);
			if (closest) {
				Game::getActionCenter()
					->addUnitAction(new IndividualOrder(worker, UnitAction::COLLECT, closest, false), player->getId());
				break;
			}
		}
	}
}
