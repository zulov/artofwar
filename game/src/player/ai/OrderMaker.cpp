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
#include "player/Possession.h"
#include "utils/OtherUtils.h"

constexpr float SEMI_CLOSE = 30.f;
constexpr float SQ_SEMI_CLOSE = SEMI_CLOSE * SEMI_CLOSE;

OrderMaker::OrderMaker(Player* player, db_nation* nation)
	: player(player), playerId(player->getId()), possession(player->getPossession()),
	  whichResource(BrainProvider::get(nation->orderPrefix[0] + "whichResource.csv")),

	  attackOrDefence(BrainProvider::get(nation->orderPrefix[1] + "attackOrDefence.csv")),
	  whereAttack(BrainProvider::get(nation->orderPrefix[2] + "whereAttack.csv")),
	  whereDefence(BrainProvider::get(nation->orderPrefix[3] + "whereDefence.csv")) {
	temp = new std::vector<Physical*>();
}

OrderMaker::~OrderMaker() {
	delete temp;
}

UnitOrder* OrderMaker::unitOrderGo(std::vector<Unit*>& subArmy, Urho3D::Vector2& center) const {
	if (subArmy.size() > 1) {
		return new GroupOrder(subArmy, UnitActionType::ORDER, cast(UnitAction::GO), center);
	}
	return new IndividualOrder(subArmy.at(0), UnitAction::GO, center);
}

UnitOrder* OrderMaker::unitOrderCollect(std::vector<Unit*>& workers, Physical* closest) const {
	if (workers.size() > 1) {
		return new GroupOrder(workers, UnitActionType::ORDER, cast(UnitAction::COLLECT), closest);
	}
	return new IndividualOrder(workers.at(0), UnitAction::COLLECT, closest);
}

void OrderMaker::action() {
	auto freeWorkers = findFreeWorkers();

	if (!freeWorkers.empty()) {
		collect(freeWorkers);
	}

	const auto env = Game::getEnvironment();

	if (possession->hasAnyFreeArmy()) {
		const auto aiInput = Game::getAiInputProvider();
		auto const resultAoD = attackOrDefence->decide(aiInput->getAttackOrDefenceInput(playerId));
		std::span<float> whereGo;
		char playerToGo = playerId;
		if (randFromTwo(resultAoD[0])) {
			playerToGo = Game::getPlayersMan()->getEnemyFor(playerId);
			whereGo = whereAttack->decide(aiInput->getWhereAttack(playerId));
		} else {
			whereGo = whereDefence->decide(aiInput->getWhereDefend(playerId));
		}
		const CenterType centerType = static_cast<CenterType>(biggestWithRand(whereGo));
		const auto posOpt = env->getCenterOf(centerType, playerToGo);

		if (posOpt.has_value()) {
			std::vector<Unit*> army = possession->getFreeArmy();
			auto center = posOpt.value();
			for (auto& subArmy : divide(army)) {
				if (!subArmy.empty()) {
					auto centerLocal = computeLocalCenter(subArmy);
					const auto dist = sqDist(centerLocal, center);

					if (dist > SQ_SEMI_CLOSE) {
						Game::getActionCenter()->addUnitAction(unitOrderGo(subArmy, center));
					} else {
						const auto unit = subArmy.at(0);
						std::vector<Physical*>* things;
						if (centerType == CenterType::BUILDING) {
							things = env->getBuildingsFromTeamNotEq(unit, -1, SEMI_CLOSE);
						} else if (centerType == CenterType::ECON) {
							const auto neights = env->getNeighboursFromTeamNotEq(unit, SEMI_CLOSE);
							//TODO perf wrzuci� predykat do srodka
							temp->clear();
							temp->reserve(neights->size());
							std::ranges::copy_if(*neights, std::back_inserter(*temp), isWorker);
							things = temp;
						} else {
							//CenterType::UNITS
							things = env->getNeighboursFromTeamNotEq(unit, SEMI_CLOSE);
						}
						semiCloseAttack(subArmy, things);
					}
				}
			}
		}
	}
}

void OrderMaker::semiCloseAttack(const std::vector<Unit*>& subArmy, const std::vector<Physical*>* things) const {
	if (!things->empty()) {
		const auto closest = Game::getEnvironment()->closestPhysical(subArmy.at(0)->getMainGridIndex(), things,
		                                                             belowClose, true);
		if (closest) {
			Game::getActionCenter()->addUnitAction(
				new GroupOrder(subArmy, UnitActionType::ORDER,
				               cast(UnitAction::ATTACK), closest));
		}
	}
}

std::vector<Unit*> OrderMaker::findFreeWorkers() const {
	std::vector<Unit*> freeWorkers;
	std::ranges::copy_if(possession->getWorkers(),
	                     std::back_inserter(freeWorkers),
	                     isFreeWorker);
	return freeWorkers;
}

Physical* OrderMaker::closetInRange(Unit* worker, int resourceId) {
	float prevRadius = -1.f;
	for (const auto radius : {64.f, 128.f, 256.f}) {
		const auto list = Game::getEnvironment()->getResources(worker->getPosition(), resourceId, radius, prevRadius);
		const auto closest = Game::getEnvironment()->closestPhysical(worker->getMainGridIndex(), list, belowClose,
		                                                             false);
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
			Game::getActionCenter()->addUnitAction(unitOrderCollect(workers, closest));
		} else {
			rest.insert(rest.end(), workers.begin(), workers.end());
		}
		workers.clear();
	}
}

void OrderMaker::collect(std::vector<Unit*>& freeWorkers) {
	const auto input = Game::getAiInputProvider()->getResourceInput(playerId);
	const auto result = whichResource->decide(input);

	unsigned char resHistogram[RESOURCES_SIZE];
	std::fill_n(resHistogram, RESOURCES_SIZE, 0);
	for (int i = 0; i < freeWorkers.size(); ++i) {
		//TODO to powinno byc posortowane wed�ugo ilo�ci? 
		const auto resourceId = biggestWithRand(result); //TODO perf tutaj tylko losowaca sortowanie wyciagnac wy�ej
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
