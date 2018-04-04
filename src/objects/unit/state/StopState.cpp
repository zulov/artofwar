#include "StopState.h"
#include "../Unit.h"


StopState::StopState() {
	transitions[static_cast<char>(UnitStateType::DEFEND)] = true;
	transitions[static_cast<char>(UnitStateType::DEAD)] = true;
	transitions[static_cast<char>(UnitStateType::GO)] = true;
	transitions[static_cast<char>(UnitStateType::PATROL)] = true;
	transitions[static_cast<char>(UnitStateType::FOLLOW)] = true;
	transitions[static_cast<char>(UnitStateType::ATTACK)] = true;
	transitions[static_cast<char>(UnitStateType::MOVE)] = true;
	transitions[static_cast<char>(UnitStateType::CHARAGE)] = true;
	transitions[static_cast<char>(UnitStateType::COLLECT)] = true;
}

StopState::~StopState() = default;

void StopState::onStart(Unit* unit) {
	unit->removeAim();
}

void StopState::onStart(Unit* unit, ActionParameter& parameter) {

}

void StopState::onEnd(Unit* unit) {
	State::onEnd(unit);
}

void StopState::execute(Unit* unit) {
	State::execute(unit);
}
