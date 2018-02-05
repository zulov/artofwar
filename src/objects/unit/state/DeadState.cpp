#include "DeadState.h"
#include "../Unit.h"


DeadState::DeadState() = default;


DeadState::~DeadState() = default;

void DeadState::onStart(Unit* unit) {

}

void DeadState::onStart(Unit* unit, ActionParameter& parameter) {
}


void DeadState::onEnd(Unit* unit) {
}

void DeadState::execute(Unit* unit) {

	unit->alive = false;
}
