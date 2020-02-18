#include "UnitOrder.h"
#include "enums/UnitAction.h"

void UnitOrder::execute() {
	switch (action) {
	case UnitAction::GO:
		return addTargetAim();
	case UnitAction::FOLLOW:
		if (toUse &&toUse
		->
		isAlive()
		)
		{
			addFollowAim();
		}
		break;
	case UnitAction::CHARGE:
		return addChargeAim();
	case UnitAction::ATTACK:
		return addAttackAim();
	case UnitAction::DEAD:
		return addDeadAim();
	case UnitAction::DEFEND:
		return addDefendAim();
	case UnitAction::COLLECT:
		return addCollectAim();
	case UnitAction::STOP:
		return addStopAim();
	}
}
