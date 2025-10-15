#include "OrderMaker.h"

#include "ActionCenter.h"
#include "AiUtils.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "math/MathUtils.h"
#include "objects/PhysicalUtils.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/IndividualOrder.h"
#include "objects/unit/order/UnitConst.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "env/Environment.h"
#include "stats/AiInputProvider.h"
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "objects/unit/GroupUtils.h"
#include "objects/unit/order/GroupOrder.h"
#include "env/influence/CenterType.h"
#include "player/Possession.h"
#include "utils/DebugUtils.h"
#include "utils/OtherUtils.h"

constexpr float SEMI_CLOSE = 30.f;
constexpr float SQ_SEMI_CLOSE = SEMI_CLOSE * SEMI_CLOSE;

OrderMaker::OrderMaker(Player* player, db_nation* nation)
	: player(player), playerId(player->getId()), possession(player->getPossession()),
	  whichResource(BrainProvider::get(nation->orderPrefix[0] + "whichResource.csv")),
	  attackOrDefence(BrainProvider::get(nation->orderPrefix[1] + "attackOrDefence.csv")),
	  whereAttack(BrainProvider::get(nation->orderPrefix[2] + "whereAttack.csv")),
	  whereDefence(BrainProvider::get(nation->orderPrefix[3] + "whereDefence.csv")),
	  aiInput(Game::getAiInputProvider()) {
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

std::vector<Physical*>* OrderMaker::getThingsToAttack(const CenterType centerType, Unit* const unit) {
	if (centerType == CenterType::BUILDING) {
		return Game::getEnvironment()->getBuildingsFromTeamNotEq(unit, -1, SEMI_CLOSE);
	}
	if (centerType == CenterType::ECON) {
		const auto neights = Game::getEnvironment()->getNeighboursFromTeamNotEq(unit, SEMI_CLOSE);
		//TODO perf wrzuciæ predykat do srodka
		temp->clear();
		temp->reserve(neights->size());
		std::ranges::copy_if(*neights, std::back_inserter(*temp), isWorker);
		return temp;
	} //CenterType::UNITS
	return Game::getEnvironment()->getNeighboursFromTeamNotEq(unit, SEMI_CLOSE);
}

void OrderMaker::armyAction() {
	const auto enemy = Game::getPlayersMan()->getEnemyFor(playerId);
	auto const resultAoD = attackOrDefence->decide(aiInput->getAttackOrDefenceInput(player, enemy));
	std::span<float> whereGo;
	char playerToGo = playerId;
	if (randFromTwo(resultAoD[0])) {
		playerToGo = enemy->getId();
		whereGo = whereAttack->decide(aiInput->getWhereAttack(player, enemy));
	} else {
		whereGo = whereDefence->decide(aiInput->getWhereDefend(player, enemy));
	}
	const CenterType centerType = static_cast<CenterType>(biggestWithRand(whereGo));
	const auto posOpt = Game::getEnvironment()->getCenterOf(centerType, playerToGo);
	if (!posOpt.has_value()) { return; }

	std::vector<Unit*> army = possession->getFreeArmy();
	auto target = posOpt.value();
	for (auto& subArmy : divide(army)) { //TODO kazda sub armia moze miec inny cel
		auto armyCenter = computeCenter(subArmy);
		const auto dist = sqDist(armyCenter, target);

		if (dist > SQ_SEMI_CLOSE) {
			Game::getActionCenter()->addUnitAction(unitOrderGo(subArmy, target));
		} else {
			const auto unit = subArmy.at(0);
			semiCloseAttack(subArmy, getThingsToAttack(centerType, unit));
		}
	}
}

void OrderMaker::action() {
	auto freeWorkers = findFreeWorkers();

	if (!freeWorkers.empty()) {
		collect(freeWorkers);
	} //TODO change busy worker

	if (possession->hasAnyFreeArmy()) {
		armyAction();
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
	const auto env = Game::getEnvironment();
	for (const auto radius : {64.f, 128.f, 256.f}) {
		const auto list = env->getResources(worker->getPosition(), resourceId, radius, prevRadius);
		const auto closest = env->closestPhysical(worker->getMainGridIndex(), list, belowClose,
		                                          false);
		if (closest) {
			return closest;
		}
		prevRadius = radius;
	}
	return nullptr;
}

void OrderMaker::actCollect(char resId, std::vector<Unit*>& rest, std::vector<Unit*>& workers) {
	if (!workers.empty()) {
		resHistogram[resId] -= workers.size();
		const auto closest = closetInRange(workers.at(0), resId);
		if (closest) {
			Game::getActionCenter()->addUnitAction(unitOrderCollect(workers, closest));
		} else {
			rest.insert(rest.end(), workers.begin(), workers.end());
		}
		workers.clear();
	}
}

std::vector<Unit*> OrderMaker::getSubGroup(std::vector<std::vector<Unit*>>& groups, unsigned char n) {
	int biggestVal = groups[0].size();
	int bigestIdx = 0;
	for (int i = 1; i < groups.size(); ++i) {
		if (groups[i].size() > biggestVal) {
			bigestIdx = i;
			biggestVal = groups[i].size();
		}
	}
	return moveNLastElements(groups[bigestIdx], n);
}

void OrderMaker::collect(std::vector<Unit*>& freeWorkers) {
	const auto enemy = Game::getPlayersMan()->getEnemyFor(playerId);

	const auto result = whichResource->decide(aiInput->getResourceInput(player, enemy));
	if (freeWorkers.size() == 1) {
		const auto resourceId = biggestWithRand(result);
		const auto worker = freeWorkers.at(0);
		const auto closest = closetInRange(worker, resourceId);
		if (closest) {
			Game::getActionCenter()->addUnitAction(new IndividualOrder(worker, UnitAction::COLLECT, closest));
		}
		return;
	}

	std::fill_n(resHistogram, RESOURCES_SIZE, 0);

	for (int i = 0; i < freeWorkers.size(); ++i) {
		const auto resourceId = biggestWithRand(result); //TODO perf tutaj tylko losowac
		++resHistogram[resourceId];
	}
	int all = freeWorkers.size();
	std::vector<Unit*> rest;
	auto groups = divide(freeWorkers);

	while (all > 0) {
		auto maxHistogram = std::ranges::max_element(resHistogram);
		auto resId = maxHistogram - std::begin(resHistogram);

		auto workers = getSubGroup(groups, *maxHistogram);
		const auto closest = closetInRange(workers.at(0), resId);

		if (closest) {
			Game::getActionCenter()->addUnitAction(unitOrderCollect(workers, closest));
			*maxHistogram -= workers.size();
		} else {
			rest.insert(rest.end(), workers.begin(), workers.end());
		}
		all -= workers.size();
	}
	if (rest.size() > 0) { COUNT("collect rest"); }
}
