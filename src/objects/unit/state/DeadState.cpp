#include "DeadState.h"
#include "../Unit.h"


DeadState::DeadState() {
}


DeadState::~DeadState() {
}

void DeadState::onStart(Unit* unit) {
	unit->alive = false;
}
void DeadState::onStart(Unit* unit, ActionParameter* parameter) {
}


void DeadState::onEnd(Unit* unit) {
}

void DeadState::execute(Unit* unit) {
}
