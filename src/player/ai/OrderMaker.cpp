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

OrderMaker::OrderMaker(Player* player, db_nation* nation)
	: player(player),
	  whichResource(BrainProvider::get(std::string(nation->orderPrefix[0].CString()) + "whichResource_w.csv")),
	  attackThreshold(ThresholdProvider::get(std::string(nation->orderThresholdPrefix[0].CString()) + "attack_t.csv")) {
}

void OrderMaker::action() {
	auto freeWorkers = findFreeWorkers();

	if (!freeWorkers.empty()) {
		collect(freeWorkers);
	}
	auto& possesion = player->getPossession();

	bool ifAttack = attackThreshold->ifDo(possesion.getFreeArmyMetrics());
	if (ifAttack) {
		char id = attackThreshold->getBest(possesion.getFreeArmyMetrics());

		const char enemy = Game::getPlayersMan()->getEnemyFor(player->getId());
		auto posOpt = Game::getEnvironment()->getCenterOf(CenterType(id), enemy);
		if (posOpt.has_value()) {
			std::vector<Unit*> army = possesion.getFreeArmy();
			//TODO perf podzieliæ armie
			if (!army.empty()) {
				Game::getActionCenter()->addUnitAction(
					new GroupOrder(army, UnitActionType::ORDER, cast(UnitAction::GO), posOpt.value()), player->getId());
			}
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
	return Game::getEnvironment()->closestPhysical(worker, list, belowClose);
}

void OrderMaker::collect(std::vector<Unit*>& workers) {
	const auto input = Game::getAiInputProvider()->getResourceInput(player->getId());
	const auto result = whichResource->decide(input);
	//TODO perf pogrupowac workerów a nie po jednym
	for (auto worker : workers) {
		const auto resourceId = biggestWithRand(result); //TODO perf tutaj tylko losowaca sortowanie wyciagnac wy¿ej

		for (auto radius : {64.f, 128.f, 256.f}) {
			const auto closest = closetInRange(worker, resourceId, radius); //TODO perf nie sprawdzac tego co wczesniej
			if (closest) {
				Game::getActionCenter()
					->addUnitAction(new IndividualOrder(worker, UnitAction::COLLECT, closest), player->getId());
				break;
			}
		}
	}
}
