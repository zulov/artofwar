#include "FutureOrder.h"

FutureOrder::FutureOrder(const Urho3D::Vector2& vector, const Physical* physical, UnitOrder action)
	: vector(vector),
	physical(physical),
	action(action) {
}

FutureOrder::FutureOrder(const Urho3D::Vector2& vector, UnitOrder action)
	: vector(vector),
	action(action) {
}

FutureOrder::FutureOrder(const Physical* physical, UnitOrder action)
	: physical(physical),
	action(action) {
}

bool FutureOrder::expired() const {
	return physical != nullptr && !physical->isAlive();
}

void FutureOrder::execute(m) {
	switch (action) {
	case UnitOrder::GO:
		return addTargetAim(vector, append);
	case UnitOrder::FOLLOW:
		if (toUse && toUse->isAlive()) {
			addFollowAim(toUse, append);
		}
		break;
	case UnitOrder::CHARGE:
		return addChargeAim(vector, append);
	case UnitOrder::ATTACK:
		return addAttackAim(toUse, append);
	case UnitOrder::DEAD:
		return addDeadAim();
	case UnitOrder::DEFEND:
		return addDefendAim();
	}
}
