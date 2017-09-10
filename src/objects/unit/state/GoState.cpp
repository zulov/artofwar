#include "GoState.h"


GoState::GoState() {
	transitions.insert(UnitStateType::STOP);
	transitions.insert(UnitStateType::DEFEND);
	transitions.insert(UnitStateType::DEAD);
}


GoState::~GoState() {
}

void GoState::onStart(Unit* unit) {
}

void GoState::onEnd(Unit* unit) {
}

void GoState::execute(Unit* unit) {
}
