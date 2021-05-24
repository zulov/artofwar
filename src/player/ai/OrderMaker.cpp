#include "OrderMaker.h"

#include "ActionCenter.h"
#include "AiUtils.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "math/MathUtils.h"
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
#include "objects/unit/GroupUtils.h"
#include "objects/unit/order/GroupOrder.h"
#include "objects/unit/order/enums/UnitActionType.h"
#include "threshold/Threshold.h"
#include "threshold/ThresholdProvider.h"

OrderMaker::OrderMaker(Player* player, db_nation* nation)
	: player(player),
	  whichResource(BrainProvider::get(std::string(nation->orderPrefix[0].CString()) + "whichResource_w.csv")),
	  attackThreshold(
		  ThresholdProvider::get(std::string(nation->orderThresholdPrefix[0].CString()) + "attack_t.csv")) {
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
			auto subArmies = divide(army);
			for (auto subArmy : subArmies) {
				if (!subArmy.empty()) {
					auto center = computeLocalCenter(subArmy);
					auto d = sqDist(posOpt.value(), center);
					if (d > 10 * 10) {
						Game::getActionCenter()->addUnitAction(
							new GroupOrder(army, UnitActionType::ORDER, cast(UnitAction::GO), posOpt.value()),
							player->getId());
					} else {
						//TODO tryToAttack
						// Game::getActionCenter()->addUnitAction(
						// 	new GroupOrder(army, UnitActionType::ORDER, cast(UnitAction::ATTACK), posOpt.value()),
						// 	player->getId());
					}
				}
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

Physical* OrderMaker::closetInRange(Unit* worker, int resourceId) {
	float prevRadius = -1.f;
	for (auto radius : {64.f, 128.f, 256.f}) {
		const auto list = Game::getEnvironment()->getResources(worker->getPosition(), resourceId, radius, prevRadius);
		const auto closest = Game::getEnvironment()->closestPhysical(worker, list, belowClose, radius * radius);
		if (closest) {
			return closest;
		}
		prevRadius = radius;
	}
	return nullptr;
}

void OrderMaker::actCollect(unsigned char& resHistogram, char resId, std::vector<Unit*>& rest,
                            std::vector<Unit*>& workers) {
	if (!workers.empty()) {
		resHistogram -= workers.size();
		const auto closest = closetInRange(workers.at(0), resId);
		if (closest) {
			if (workers.size() <= 1) {
				Game::getActionCenter()
					->addUnitAction(new IndividualOrder(workers.at(0), UnitAction::COLLECT, closest),
					                player->getId());
			} else {
				Game::getActionCenter()
					->addUnitAction(
						new GroupOrder(workers, UnitActionType::ORDER, cast(UnitAction::COLLECT), closest),
						player->getId());
			}
		} else {
			rest.insert(rest.end(), workers.begin(), workers.end());
		}
		workers.clear();
	}
}

void OrderMaker::collect(std::vector<Unit*>& freeWorkers) {
	const auto input = Game::getAiInputProvider()->getResourceInput(player->getId());
	const auto result = whichResource->decide(input);
	//TODO perf pogrupowac workerów a nie po jednym
	unsigned char resHistogram[RESOURCES_SIZE];
	std::fill_n(resHistogram, RESOURCES_SIZE, 0);
	for (int i = 0; i < freeWorkers.size(); ++i) {
		//TODO to powinno byc posortowane wed³ugo iloœci? 
		const auto resourceId = biggestWithRand(result); //TODO perf tutaj tylko losowaca sortowanie wyciagnac wy¿ej
		++resHistogram[resourceId];
	}
	char idx = 0;
	std::vector<Unit*> rest;
	for (auto& workersGroup : divide(freeWorkers)) {
		//TODO close divide
		std::vector<Unit*> workers;
		for (int i = 0; i < workersGroup.size(); ++i) {
			if (workers.size() < resHistogram[idx]) {
				workers.push_back(workersGroup[i]);
			} else {
				actCollect(resHistogram[idx], idx, rest, workers);
				--i;
				++idx;
			}
			if (idx >= RESOURCES_SIZE) {
				break;
			}
		}
		actCollect(resHistogram[idx], idx, rest, workers);

	}
}
