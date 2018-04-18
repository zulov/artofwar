#include "StopState.h"
#include "../Unit.h"


StopState::StopState() {
	nextStates[static_cast<char>(UnitStateType::DEFEND)] = true;
	nextStates[static_cast<char>(UnitStateType::DEAD)] = true;
	nextStates[static_cast<char>(UnitStateType::GO)] = true;
	nextStates[static_cast<char>(UnitStateType::PATROL)] = true;
	nextStates[static_cast<char>(UnitStateType::FOLLOW)] = true;
	nextStates[static_cast<char>(UnitStateType::ATTACK)] = true;
	nextStates[static_cast<char>(UnitStateType::MOVE)] = true;
	nextStates[static_cast<char>(UnitStateType::CHARAGE)] = true;
	nextStates[static_cast<char>(UnitStateType::COLLECT)] = true;
}

StopState::~StopState() = default;

void StopState::onStart(Unit* unit) {
	unit->clearAims();
}

void StopState::onStart(Unit* unit, ActionParameter& parameter) {

}

void StopState::onEnd(Unit* unit) {
	State::onEnd(unit);
}

void StopState::execute(Unit* unit) {
	State::execute(unit);
}
