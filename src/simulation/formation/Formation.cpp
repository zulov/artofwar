#include "Formation.h"
#include "Game.h"
#include "commands/CommandList.h"
#include "commands/action/ActionCommand.h"
#include "commands/action/FormationAction.h"
#include "objects/unit/Unit.h"
#include "objects/unit/aim/FutureAim.h"
#include <algorithm>
#include <iostream>
#include "simulation/env/Enviroment.h"
#include <unordered_set>
#include <numeric>
#include <chrono>

Formation::Formation(short _id, std::vector<Physical*>* _units, FormationType _type, Vector2& _direction) : id(_id),
	type(_type), direction(_direction), state(FormationState::FORMING) {

	for (auto value : *_units) {
		auto unit = dynamic_cast<Unit*>(value);
		unit->clearAims();
		units.push_back(unit);
	}

	if (!units.empty()) {
		updateIds();
		updateCenter();
	} else {
		changeState(FormationState::EMPTY);
	}
}

Formation::~Formation() = default;

Vector2 Formation::computeLocalCenter() {
	Vector2 localCenter = Vector2::ZERO;
	for (auto unit : units) {
		const auto pos = unit->getPosition();
		localCenter.x_ += pos->x_;
		localCenter.y_ += pos->z_;
	}
	localCenter /= units.size();
	return localCenter;
}

void Formation::setNewLeader(Vector2& localCenter) {
	int maxDist = 9999999;
	leader = nullptr;
	for (auto& unit : units) {
		const auto pos = unit->getPosition();
		const auto currentPos = Vector2(pos->x_, pos->z_);

		const auto sqDist = (currentPos - localCenter).LengthSquared();
		if (sqDist < maxDist) {
			leader = unit;
			maxDist = sqDist;
		}
	}
}

void Formation::electLeader() {
	Vector2 localCenter = computeLocalCenter();
	setNewLeader(localCenter);
	if (oldLeader != nullptr
		&& leader != oldLeader
		&& !futureOrders.empty()) {

		if (oldLeader->getFormation() == id) {
			oldLeader->clearAims();
		}
		changeState(FormationState::FORMING);
	}
	oldLeader = leader;
}

void Formation::updateIds() {
	//if (changed) {
	auto start = std::chrono::system_clock::now();
	updateSizes();
	electLeader(); //TODO pierwsze ustawic id normlanie wybradc lidera, reszte Id skasowac i dopiero pozniej reszta

	updateCenter();
	short k = 0;
	for (auto unit : units) {
		unit->setFormation(id);
		if (unit != leader) {
			unit->setPositionInFormation(-1);
		} else {
			unit->setPositionInFormation(k);
		}
		++k;
	}

	auto env = Game::get()->getEnviroment();
	std::unordered_map<int, std::vector<short>> bucketToIds;
	for (int i = 0; i < units.size(); ++i) {
		auto pos = getPositionFor(i);
		int bucketForI = env->getIndex(pos);

		auto it = bucketToIds.find(bucketForI);
		if (it != bucketToIds.end()) {
			it->second.push_back(i);
		} else {
			bucketToIds.emplace(bucketForI, std::vector<short>(4));
			bucketToIds.at(bucketForI).push_back(i);
		}
	}

	std::vector<short> tempVec(units.size());
	std::iota(tempVec.begin(), tempVec.end(), 0);
	std::set<short> v(tempVec.begin(), tempVec.end());

	short leaderID = leader->getPositionInFormation();
	auto test = v.find(leaderID);
	if (test != v.end()) {
		v.erase(test); // remove leaderID
	}

	for (auto unit : units) {
		if (leader == unit) {
			continue;
		}
		int bucketId = unit->getBucketIndex(-1);

		auto it = bucketToIds.find(bucketId);
		if (it != bucketToIds.end()) {
			for (auto id : bucketToIds.at(bucketId)) {
				auto it2 = v.find(id);
				if (it2 != v.end()) {
					//bucketToIds.erase(it);
					v.erase(it2);
					unit->setPositionInFormation(id);
					break;
				}
			}
		}
	}
	std::vector<short> output(v.begin(), v.end());
	int i = 0;
	for (auto unit : units) {
		if (unit->getPositionInFormation() == -1) {
			unit->setPositionInFormation(output[i]);
			++i;
			//	std::cout << "####" << unit->getPositionInFormation() << std::endl;
		}
	}


	changed = false;
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start);
	//std::cout << "Update ids complated at " << duration.count() << " micros" << std::endl;
	//}
}

void Formation::updateSizes() {
	sideA = sqrt(units.size()) + 0.5;
	sideB = units.size() / sideA;

	sizeA = (sideA - 1) * sparsity;
	sizeB = (sideB - 1) * sparsity;
}

void Formation::calculateNotWellFormed() {
	notWellFormed = 0;
	notWellFormedExact = 0;
	for (auto unit : units) {
		auto pos = unit->getPosition();

		auto currentPos = Vector2(pos->x_, pos->z_);
		auto desiredPos = getPositionFor(unit->getPositionInFormation());
		auto sqDist = (currentPos - desiredPos).LengthSquared();
		if (sqDist > 2 * 2) {
			notWellFormed += 1;
		}
		if (sqDist > 1) {
			notWellFormedExact += 1;
		}
	}
	notWellFormed /= units.size();
	notWellFormedExact /= units.size();
}

void Formation::innerUpdate() {
	updateUnits();

	if (!units.empty()) {
		updateIds();
		updateCenter();
		calculateNotWellFormed();
	} else {
		changeState(FormationState::EMPTY);
	}
}

void Formation::update() {
	switch (state) {
	case FormationState::FORMING:
		innerUpdate();
		if (notWellFormed < theresholedMin) {
			changeState(FormationState::MOVING);
			if (!futureOrders.empty()) {
				const auto& futureOrder = futureOrders[0];
				Game::get()->getActionCommandList()->add(new FormationAction(this,
				                                                             futureOrder.action,
				                                                             new Vector2(futureOrder.vector)
				                                                            ));
				futureOrders.erase(futureOrders.begin()); //TODO to zachowaæ

			}
		}
		break;
	case FormationState::MOVING:
		innerUpdate();
		if (notWellFormed > theresholedMax) {
			changeState(FormationState::FORMING);
		} else if (notWellFormedExact < theresholedMin
			&& !leader->hasAim()) {
			if (futureOrders.empty()) {
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
		break;
	}
}

void Formation::changeState(FormationState newState) {
	state = newState;
}

Vector2 Formation::getPositionFor(short id) {
	const int columnThis = id % sideA;
	const int rowThis = id / sideA;

	short leaderID = leader->getPositionInFormation();

	const int columnLeader = leaderID % sideA;
	const int rowLeader = leaderID / sideA;

	const int column = columnThis - columnLeader;
	const int row = rowThis - rowLeader;

	return center - Vector2(column * sparsity, row * sparsity);
}

float Formation::getPriority(int id) const {
	return 1;
}

std::optional<Physical*> Formation::getLeader() {
	if (state != FormationState::REACHED && units.size() > leader->getPositionInFormation()) {
		return leader;
	}
	return std::nullopt;
}

void Formation::addAim(const Vector2& _vector, const Physical* _physical,
                       OrderType _action, bool append) {
	if (!append) {
		futureOrders.clear();
	}
	futureOrders.emplace_back(_vector, _physical, _action);
}

size_t Formation::getSize() {
	return units.size();
}

void Formation::semiReset() {
	notWellFormed = 1;
	notWellFormedExact = 1;
	changed = true;
	futureOrders.clear();
	changeState(FormationState::FORMING);
}

void Formation::updateUnits() {
	units.erase(
	            std::remove_if(
	                           units.begin(), units.end(),
	                           [this](Unit* unit)
	                           {
		                           bool ifErase = !unit->isAlive() || unit->getFormation() != id;
		                           if (ifErase) {
			                           if (unit->getFormation() == id) {
				                           unit->resetFormation();
			                           }
			                           changed = true;
		                           }
		                           return ifErase;
	                           }),
	            units.end());
}

void Formation::updateCenter() {
	Vector3* leaderPos = leader->getPosition();
	center = Vector2(leaderPos->x_, leaderPos->z_);
}
