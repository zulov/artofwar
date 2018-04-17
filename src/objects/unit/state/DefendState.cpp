#include "DefendState.h"
#include "../Unit.h"


DefendState::DefendState() {
	transitions[static_cast<char>(UnitStateType::STOP)] = true;
	transitions[static_cast<char>(UnitStateType::DEAD)] = true;
}


DefendState::~DefendState() = default;

void DefendState::onStart(Unit* unit) {
	unit->clearAims();
}

void DefendState::onStart(Unit* unit, ActionParameter& parameter) {
}


void DefendState::onEnd(Unit* unit) {
	State::onEnd(unit);
}

void DefendState::execute(Unit* unit) {
	State::execute(unit);
}
