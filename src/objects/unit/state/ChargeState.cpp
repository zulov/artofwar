#include "ChargeState.h"


ChargeState::ChargeState() {
	transitions.insert(UnitStateType::DEAD);
}


ChargeState::~ChargeState() {
}

void ChargeState::onStart(Unit* unit) {
}
void ChargeState::onStart(Unit* unit, ActionParameter* parameter) {
}

void ChargeState::onEnd(Unit* unit) {
}

void ChargeState::execute(Unit* unit) {
}
