#include "StopState.h"
#include "../Unit.h"


StopState::StopState() {
	transitions.insert(UnitStateType::DEFEND);
	transitions.insert(UnitStateType::DEAD);
	transitions.insert(UnitStateType::GO);
	transitions.insert(UnitStateType::PATROL);
	transitions.insert(UnitStateType::FOLLOW);
	transitions.insert(UnitStateType::ATTACK);
	transitions.insert(UnitStateType::MOVE);
	transitions.insert(UnitStateType::CHARAGE);
	transitions.insert(UnitStateType::COLLECT);
}

StopState::~StopState() = default;

void StopState::onStart(Unit* unit) {
	unit->removeAim();
}

void StopState::onStart(Unit* unit, ActionParameter* parameter) {

}

void StopState::onEnd(Unit* unit) {
}

void StopState::execute(Unit* unit) {
}
