#include "StopState.h"
#include "../Unit.h"


StopState::StopState() {
	transitions.insert(UnitStateType::DEFEND);
	transitions.insert(UnitStateType::DEAD);
}

StopState::~StopState() {
}

void StopState::onStart(Unit* unit) {
	unit->removeAim();
}

void StopState::onEnd(Unit* unit) {
}

void StopState::execute(Unit* unit) {
}
