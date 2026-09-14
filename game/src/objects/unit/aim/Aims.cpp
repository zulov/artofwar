#include "Aims.h"
#include "../order/OrderUtils.h"
#include "ChargeAim.h"
#include "FollowAim.h"
#include "TargetAim.h"
#include "objects/Physical.h"
#include "objects/unit/order/IndividualOrder.h"
#include "utils/DeleteUtils.h"

Aims::Aims() : current(nullptr) {}

Aims::~Aims() { clear(); }

Urho3D::Vector2 Aims::getDirection(Unit* unit) const { return current->getDirection(unit); }

void Aims::clearExpired() {
	removeExpired(nextAims);
	if (current != nullptr && current->expired()) {
		removeCurrentAim();
	}
}

bool Aims::process(Unit* unit) {//TODO bug to chyba źle zwraca nie wiem jaki był tego cel
	clearExpired();//TODO quick fifx?

	if (current) {
		if (current->ifReach(unit)) {
			removeCurrentAim();
			return nextAims.empty();
		}
	} else if (!nextAims.empty()) {
		auto toExecute = nextAims[0];
		
		toExecute->execute();
		nextAims.erase(nextAims.begin());
		delete toExecute;
	}

	return false;
}

void Aims::add(IndividualOrder* order) {
	assert(!order->expired());
	if (!order->getAppend()) {
		clear();
	}
	nextAims.push_back(order);
}

void Aims::clear() {
	clear_vector(nextAims);
	removeCurrentAim();
}

void Aims::removeCurrentAim() {
	delete current;
	current = nullptr;
}

std::vector<Urho3D::Vector3> Aims::getDebugLines(Unit* unit) const { return current->getDebugLines(unit); }

void Aims::set(Aim* aim) { current = aim; }

AimSaveData Aims::saveState() const { return current ? current->saveState() : AimSaveData{}; }

std::vector<UnitOrderSaveData> Aims::saveOrders(unsigned unitUid) const {
	std::vector<UnitOrderSaveData> result;
	result.reserve(nextAims.size());
	for (unsigned short i = 0; i < nextAims.size(); ++i) {
		const auto* order = nextAims[i];
		result.push_back(order->saveOrder(unitUid));
	}
	return result;
}

Aim* Aims::createAim(const AimSaveData& state, const std::unordered_map<unsigned, Physical*>& byUid) {
	if (state.kind == AimSaveKind::TARGET && !state.path.empty()) {
		auto* aim = new TargetAim(state.path);
		aim->setCurrent(state.current);
		return aim;
	} else if (state.kind == AimSaveKind::FOLLOW) {
		const auto it = byUid.find(state.targetUid);
		if (it != byUid.end()) {
			TargetAim* subTarget = nullptr;
			if (!state.path.empty()) {
				subTarget = new TargetAim(state.path);
				subTarget->setCurrent(state.current);
			}
			return new FollowAim(it->second, subTarget);
		}
	} else if (state.kind == AimSaveKind::CHARGE) {
		Urho3D::Vector2 direction(state.directionX, state.directionZ);
		return new ChargeAim(&direction);
	}
	return nullptr;
}

void Aims::loadState(Unit* unit, const AimSaveData& state, const std::vector<UnitOrderSaveData>& orders,
					 const std::unordered_map<unsigned, Physical*>& byUid) {
	clear();
	if (auto* aim = createAim(state, byUid)) {
		set(aim);
	}

	for (const auto& saved : orders) {
		if (saved.hasTarget) {
			const auto it = byUid.find(saved.targetUid);
			if (it != byUid.end()) {
				nextAims.push_back(
						new IndividualOrder(unit, static_cast<UnitAction>(saved.action), it->second, saved.append));
			}
		} else {
			nextAims.push_back(new IndividualOrder(unit, static_cast<UnitAction>(saved.action),
												   Urho3D::Vector2(saved.x, saved.z), saved.append));
		}
	}
}
