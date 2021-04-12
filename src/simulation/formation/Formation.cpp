#include "Formation.h"
#include <numeric>
#include "Game.h"
#include "math/MathUtils.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/FormationOrder.h"
#include "objects/unit/order/OrderUtils.h"
#include "objects/unit/state/StateManager.h"
#include "simulation/env/Environment.h"


Formation::Formation(short _id, const std::vector<Unit*>& _units, FormationType _type, Urho3D::Vector2 _direction) :
	id(_id), type(_type), state(FormationState::FORMING), direction(_direction), units(_units) {

	for (auto* unit : units) {
		unit->clearAims();
		StateManager::toDefaultState(unit);
	}

	if (!units.empty()) {
		updateIds();
		setCenter();
	} else {
		changeState(FormationState::EMPTY);
	}
	levelOfReach = new char[units.size()];
	std::fill_n(levelOfReach, units.size(), 0);
}

Formation::~Formation() {
	delete[] levelOfReach;
}

Urho3D::Vector2 Formation::computeLocalCenter() {
	auto localCenter = Urho3D::Vector2::ZERO;
	for (auto unit : units) {
		const auto pos = unit->getPosition();
		localCenter.x_ += pos.x_;
		localCenter.y_ += pos.z_;
	}
	localCenter /= units.size();
	return localCenter;
}

void Formation::chooseLeader(Urho3D::Vector2& localCenter) {
	int maxDist = 99999;
	leader = nullptr;
	for (auto unit : units) {
		const auto dist = sqDist(localCenter, unit->getPosition());
		if (dist < maxDist) {
			leader = unit;
			maxDist = dist;
		}
	}
}

void Formation::electLeader() {
	auto center = computeLocalCenter();
	chooseLeader(center);

	if (oldLeader != nullptr
		&& leader != oldLeader
		&& !unitOrders.empty()) {

		if (oldLeader->getFormation() == id) {
			oldLeader->clearAims();
		}
		changeState(FormationState::FORMING);
	}
	oldLeader = leader;
}

void Formation::setFormationClearPosition() {
	for (auto unit : units) {
		unit->setFormation(id);
		unit->setPositionInFormation(-1);
	}
}

void Formation::setPosInFormationForLeader() const {
	short posInFormation = (short)((sideB - 1) / 2) * sideA + (short)(sideA / 2.0 + 0.5);
	if (posInFormation >= units.size()) {
		posInFormation = units.size() - 1;
	}
	leader->setPositionInFormation(posInFormation);
}

void Formation::updateIds() {
	if (changed) {
		updateSideSize();
		electLeader();
		setCenter();
		setFormationClearPosition();

		setPosInFormationForLeader();

		const auto env = Game::getEnvironment();
		std::unordered_map<int, std::vector<short>> bucketToIds;
		for (int i = 0; i < units.size(); ++i) {
			if (leader->getPositionInFormation() == i) {
				continue;
			}
			auto pos = getPositionFor(i);
			int bucketForI = env->getIndex(pos);

			auto it = bucketToIds.find(bucketForI);
			if (it != bucketToIds.end()) {
				it->second.push_back(i);
			} else {
				auto inserted = bucketToIds.emplace(bucketForI, std::vector<short>());
				inserted.first->second.reserve(4);
				inserted.first->second.push_back(i);
			}
		}

		std::vector<short> tempVec(units.size());
		std::iota(tempVec.begin(), tempVec.end(), 0);

		const short leaderID = leader->getPositionInFormation();
		tempVec[leaderID] = -1;
		short restToAssign = tempVec.size() - 1;
		for (auto unit : units) {
			if (leader == unit) {
				continue;
			}
			int bucketId = unit->getMainCell();
			const auto pos = unit->getPosition();
			const auto currentPos = Urho3D::Vector2(pos.x_, pos.z_);

			auto it = bucketToIds.find(bucketId);
			if (it != bucketToIds.end()) {
				short bestId = -1;
				float bestSize = 99999; //TODO do zast¹pienia tym z unit
				for (int i = 0; i < it->second.size(); ++i) {
					const auto id = it->second[i];
					if (tempVec[id] != -1) {
						const auto dist = sqDist(currentPos, getPositionFor(id));
						if (dist < bestSize) {
							bestSize = dist;
							bestId = i;
						}
					}
				}

				if (bestId >= 0) {
					--restToAssign;
					tempVec[it->second[bestId]] = -1;
					unit->setPositionInFormation(it->second[bestId]);
					it->second.erase(it->second.begin() + bestId);
				}
			}
		}
		std::vector<short> output;
		output.reserve(restToAssign);
		std::copy_if(tempVec.begin(), tempVec.end(), std::back_inserter(output), [](const short i) { return i >= 0; });

		int i = 0;
		for (auto unit : units) {
			if (unit->getPositionInFormation() == -1) {
				unit->setPositionInFormation(output[i]);
				++i;
			}
		}
		changed = false;
	}
}

void Formation::updateSideSize() {
	sideA = sqrt(units.size()) + 0.5;
	sideB = units.size() / sideA;
}

void Formation::calculateNotWellFormed() {
	notWellFormed = 0;
	notWellFormedExact = 0;
	for (auto unit : units) {
		const auto desiredPos = getPositionFor(unit->getPositionInFormation());
		const auto dist = sqDist(unit->getPosition(), desiredPos);

		if (dist < 0.5f) {
			levelOfReach[unit->getPositionInFormation()] = 0;
		} else if (dist < 2 * 2) {
			notWellFormedExact += 1;
			levelOfReach[unit->getPositionInFormation()] = 1;
		} else {
			notWellFormed += 1;
			notWellFormedExact += 1;
			levelOfReach[unit->getPositionInFormation()] = 2;
		}
	}
	notWellFormed /= units.size();
	notWellFormedExact /= units.size();
}

void Formation::innerUpdate() {
	removeExpired(unitOrders);
	updateUnits();

	if (!units.empty()) {
		updateIds();
		setCenter();
		calculateNotWellFormed();
	} else {
		changeState(FormationState::EMPTY);
	}
}

void Formation::stopAllBesideLeader() {
	for (auto unit : units) {
		if (unit != leader) {
			StateManager::toDefaultState(unit);
		}
	}
}

void Formation::update() {
	switch (state) {
	case FormationState::FORMING:
		innerUpdate();
		if (notWellFormed < thresholdMin) {
			changeState(FormationState::MOVING);
			if (!unitOrders.empty()) {
				unitOrders[0]->execute();
				stopAllBesideLeader();
				delete unitOrders[0];
				unitOrders.erase(unitOrders.begin()); //TODO to zachowaæ, chyba jednak nie trzeba
			}
		}
		break;
	case FormationState::MOVING:
		innerUpdate();
		if (notWellFormed > thresholdMax) {
			changeState(FormationState::FORMING);
		} else if (notWellFormedExact < thresholdMin
			&& !leader->hasAim()) {
			if (unitOrders.empty()) {
				changeState(FormationState::REACHED);
			} else {
				changeState(FormationState::FORMING);
			}
		}
		break;
	case FormationState::REACHED:
		for (auto unit : units) {
			unit->resetFormation();
		}
		units.clear();
		changeState(FormationState::EMPTY);
		break;
	}
}

void Formation::remove() {
	changeState(FormationState::REACHED);
}

void Formation::changeState(FormationState newState) {
	state = newState;
}

Urho3D::Vector2 Formation::getPositionFor(short id) const {
	const short columnThis = id % sideA;
	const short rowThis = id / sideA;
	const short leaderID = leader->getPositionInFormation();

	const short columnLeader = leaderID % sideA;
	const short rowLeader = leaderID / sideA;

	const short column = columnThis - columnLeader;
	const short row = rowThis - rowLeader;
	//if(leaderID==id) {//TODO dla leadera trzeba cos innego wymyślić
	//return {};
	//}
	auto position = center - Urho3D::Vector2(column * sparsity, row * sparsity);
	const auto posIndex = Game::getEnvironment()->getIndex(position);
	if (Game::getEnvironment()->cellIsPassable(posIndex)) {
		return position;
	}
	//TODO perf map posIndex to closestIndex
	const int closestIndex = Game::getEnvironment()->closestPassableCell(posIndex);
	return Game::getEnvironment()->getCenter(closestIndex) - Urho3D::Vector2(column * sparsity, row * sparsity);
}

float Formation::getPriority(int id) const {
	return levelOfReach[id];
}

std::optional<Unit*> Formation::getLeader() {
	if (state != FormationState::REACHED && units.size() > leader->getPositionInFormation()) {
		return leader;
	}
	return {};
}

void Formation::addOrder(FormationOrder* order) {
	if (!order->getAppend()) {
		semiReset();
		leader->clearAims();
	}
	unitOrders.emplace_back(order);
}

size_t Formation::getSize() const {
	return units.size();
}

void Formation::semiReset() {
	notWellFormed = 1;
	notWellFormedExact = 1;
	changed = true;
	unitOrders.clear();
	std::fill_n(levelOfReach, units.size(), 0);
	changeState(FormationState::FORMING);
}

void Formation::updateUnits() {
	units.erase(
		std::remove_if(
			units.begin(), units.end(),
			[this](Unit* unit) {
				const bool ifErase = !unit->isAlive() || unit->getFormation() != id;
				if (ifErase) {
					unit->resetFormation();
					changed = true;
				}
				return ifErase;
			}),
		units.end());
}

void Formation::setCenter() {
	const auto leaderPos = leader->getPosition();
	center = {leaderPos.x_, leaderPos.z_};
}
