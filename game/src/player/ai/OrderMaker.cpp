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
#include "env/Environment.h"
#include "stats/AiInputProvider.h"
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "objects/unit/GroupUtils.h"
#include "objects/unit/order/GroupOrder.h"
#include "objects/unit/order/enums/UnitActionType.h"
#include "env/influence/CenterType.h"
#include "utils/OtherUtils.h"

constexpr float SEMI_CLOSE = 30.f;
constexpr float SQ_SEMI_CLOSE = SEMI_CLOSE * SEMI_CLOSE;

OrderMaker::OrderMaker(Player* player, db_nation* nation)
	: player(player),
	  whichResource(BrainProvider::get(nation->orderPrefix[0] + "whichResource.csv")),

	  attackOrDefence(BrainProvider::get(nation->orderPrefix[1] + "attackOrDefence.csv")),
	  whereAttack(BrainProvider::get(nation->orderPrefix[2] + "whereAttack.csv")),
	  whereDefence(BrainProvider::get(nation->orderPrefix[3] + "whereDefence.csv")) {
}

void OrderMaker::action() {
	auto freeWorkers = findFreeWorkers();

	if (!freeWorkers.empty()) {
		collect(freeWorkers);
	}

	if (player->getPossession().hasAnyFreeArmy()) {
		const auto aiInput = Game::getAiInputProvider();
		auto const resultAoD = attackOrDefence->decide(aiInput->getAttackOrDefenceInput(player->getId()));
		std::span<float> whereGo;
		char playerToGo = player->getId();
		if (randFromTwo(resultAoD[0])) {
			playerToGo = Game::getPlayersMan()->getEnemyFor(player->getId());
			whereGo = whereAttack->decide(aiInput->getWhereAttack(player->getId()));
		} else {
			whereGo = whereDefence->decide(aiInput->getWhereDefend(player->getId()));
		}
		const CenterType centerType = static_cast<CenterType>(biggestWithRand(whereGo));
		const auto posOpt = Game::getEnvironment()->getCenterOf(centerType, playerToGo);

		if (posOpt.has_value()) {
			std::vector<Unit*> army = player->getPossession().getFreeArmy();
			auto center = posOpt.value();
			for (auto& subArmy : divide(army)) {
				if (!subArmy.empty()) {
					auto centerLocal = computeLocalCenter(subArmy);
					const auto dist = sqDist(centerLocal, center);

					if (dist > SQ_SEMI_CLOSE) {
						if (subArmy.size() > 1) {
							Game::getActionCenter()->addUnitAction(
								new GroupOrder(subArmy, UnitActionType::ORDER, cast(UnitAction::GO), center),
								player->getId());
						} else {
							Game::getActionCenter()->addUnitAction(
								new IndividualOrder(subArmy.at(0), UnitAction::GO, center), player->getId());
						}
					} else {
						const auto unit = subArmy.at(0);

						if (centerType == CenterType::BUILDING) {
							const auto buildings = Game::getEnvironment()->getBuildingsFromTeamNotEq(
								unit, -1, SEMI_CLOSE);
							semiCloseAttack(subArmy, *buildings);
						} else if (centerType == CenterType::ECON) {
							const auto neights = Game::getEnvironment()->getNeighboursFromTeamNotEq(unit, SEMI_CLOSE);
							//TODO perf wrzuciæ predykat do srodka
							std::vector<Physical*> workers;
							workers.reserve(neights->size());
							auto pred = [](const Physical* physical) {
								return ((Unit*)physical)->getDbUnit()->typeWorker;
							};
							std::copy_if(neights->begin(), neights->end(), std::back_inserter(workers), pred);
							semiCloseAttack(subArmy, workers);
						} else {
							//CenterType::UNITS
							const auto neights = Game::getEnvironment()->getNeighboursFromTeamNotEq(unit, SEMI_CLOSE);
							semiCloseAttack(subArmy, *neights);
						}
					}
				}
			}
		}
	}

}

void OrderMaker::semiCloseAttack(const std::vector<Unit*>& subArmy, const std::vector<Physical*>& things) const {
	if (!things.empty()) {
		const auto closest = Game::getEnvironment()->closestPhysical(subArmy.at(0), &things, belowClose, SQ_SEMI_CLOSE);
		if (closest) {
			Game::getActionCenter()->addUnitAction(
				new GroupOrder(subArmy, UnitActionType::ORDER, cast(UnitAction::ATTACK), closest), player->getId());
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
