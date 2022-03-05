#include "Formation.h"

#include <iterator>
#include <numeric>
#include "Game.h"
#include "math/MathUtils.h"
#include "objects/unit/GroupUtils.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/FormationOrder.h"
#include "objects/unit/order/OrderUtils.h"
#include "objects/unit/state/StateManager.h"
#include "env/Environment.h"
#include "utils/OtherUtils.h"

constexpr float THRESHOLD_MIN = 0.3f;
constexpr float THRESHOLD_MAX = 0.5f;

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

void Formation::chooseLeader(Urho3D::Vector2& localCenter) {
	float maxDist = 99999.f;
	leader = nullptr;
	for (auto* unit : units) {
		const auto dist = sqDist(localCenter, unit->getPosition());
		if (dist < maxDist) {
			leader = unit;
			maxDist = dist;
		}
	}
}

void Formation::electLeader() {
	auto center = computeLocalCenter(units);
	chooseLeader(center);

	if (oldLeader != nullptr && leader != oldLeader) {
		if (oldLeader->getFormation() == id) {
			oldLeader->clearAims();
		}
		if (pendingOrder) {
			stopAllBesideLeader();
			pendingOrder->execute();
		} else if (!unitOrders.empty()) {
			//??a co to get next?
		}
		changeState(FormationState::FORMING);
	}
	oldLeader = leader;
}

void Formation::setFormationClearPosition() {
	for (auto* unit : units) {
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
		pathCache.clear();
		updateSideSize();
		electLeader();
		setCenter();
		setFormationClearPosition();

		setPosInFormationForLeader();

		auto* const env = Game::getEnvironment();
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
			int bucketId = unit->getMainGridIndex();
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
		std::ranges::copy_if(tempVec, std::back_inserter(output), [](const short i) { return i >= 0; });

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
	sideA = sqrt(units.size()) + 0.5f;
	sideB = units.size() / sideA;
}

void Formation::calculateCohesion() {
	notWellFormed = 0;
	notWellFormedExact = 0;
	for (const auto unit : units) {
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
	//pathCache.clear();
	removeExpired(unitOrders);
	if (pendingOrder && pendingOrder->expired()) {
		delete pendingOrder;
		pendingOrder = nullptr;
	}
	updateUnits();

	if (!units.empty()) {
		updateIds();
		setCenter();
		calculateCohesion();
	} else {
		changeState(FormationState::EMPTY);
	}
}

void Formation::stopAllBesideLeader() {
	pathCache.clear();
	for (auto* unit : units) {
		if (unit != leader) {
			StateManager::toDefaultState(unit);
		}
	}
}

bool Formation::isLeader(Unit* unit) const {
	return hasLeader() && unit == leader;
}

bool Formation::isMoving(Unit* unit) const {
	return state == FormationState::MOVING;
}

int Formation::getCachePath(int startIdx, int aimIndex) const {
	const auto ptr = pathCache.find(key(startIdx, aimIndex));
	if (ptr == pathCache.end()) {
		return -1;
	}
	return ptr->second;
}

void Formation::addCachePath(int startIdx, int aimIndex, int next) {
	pathCache[key(startIdx, aimIndex)] = next;
}

void Formation::update() {
	switch (state) {
	case FormationState::FORMING:
		innerUpdate();

		if (notWellFormed < THRESHOLD_MIN) {
			changeState(FormationState::MOVING);
			if (!leader->hasAim()) {
				removePending();
				if (!unitOrders.empty()) {
					pendingOrder = unitOrders[0];
					unitOrders.erase(unitOrders.begin());
					pendingOrder->execute();
					stopAllBesideLeader();
				}
			}
		}
		break;
	case FormationState::MOVING:
		innerUpdate();
		if (notWellFormed > THRESHOLD_MAX) {
			changeState(FormationState::FORMING);
		} else if (notWellFormedExact < THRESHOLD_MIN && !leader->hasAim()) {
			removePending();
			if (unitOrders.empty()) {
				changeState(FormationState::REACHED);
			} else {
				changeState(FormationState::FORMING);
			}
		}
		break;
	case FormationState::REACHED:
		for (auto* unit : units) {
			unit->resetFormation();
		}
		units.clear();
		changeState(FormationState::EMPTY);
		break;
	}
}

void Formation::removePending() {
	if (pendingOrder) {
		delete pendingOrder;
		pendingOrder = nullptr;
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
	if (leaderID == id) {
		return center;
	}
	const short columnLeader = leaderID % sideA;
	const short rowLeader = leaderID / sideA;

	const short column = columnThis - columnLeader;
	const short row = rowThis - rowLeader;

	auto position = center - Urho3D::Vector2(column * sparsity, row * sparsity);
	const auto posIndex = Game::getEnvironment()->getIndex(position);
	if (Game::getEnvironment()->cellIsPassable(posIndex)) {
		return position;
	}

	//TODO perf map posIndex to closestIndex
	const int closestIndex = Game::getEnvironment()->closestPassableCell(posIndex);
	const auto diff = position - Game::getEnvironment()->getCenter(posIndex);
	return Game::getEnvironment()->getCenter(closestIndex) + diff;
}

float Formation::getPriority(int id) const {
	return levelOfReach[id];
}

bool Formation::hasLeader() const {
	return state != FormationState::REACHED && units.size() > leader->getPositionInFormation();
}

std::optional<Unit*> Formation::getLeader() {
	if (hasLeader()) {
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
	delete pendingOrder;
	pendingOrder = nullptr;
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
					if(unit->getFormation() == id) {
						unit->resetFormation();
					}
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
