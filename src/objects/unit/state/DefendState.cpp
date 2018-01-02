#include "DefendState.h"
#include "../Unit.h"


DefendState::DefendState() {
	transitions.insert(UnitStateType::STOP);
	transitions.insert(UnitStateType::DEAD);
}


DefendState::~DefendState() = default;

void DefendState::onStart(Unit* unit) {
	unit->removeAim();
}
void DefendState::onStart(Unit* unit, ActionParameter& parameter) {
}


void DefendState::onEnd(Unit* unit) {
}

void DefendState::execute(Unit* unit) {
}
