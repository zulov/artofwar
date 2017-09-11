#include "PatrolState.h"
#include "../Unit.h"


PatrolState::PatrolState() {
	transitions.insert(UnitStateType::STOP);
	transitions.insert(UnitStateType::DEFEND);
	transitions.insert(UnitStateType::DEAD);
}


PatrolState::~PatrolState() {
}

void PatrolState::onStart(Unit* unit) {
}
void PatrolState::onStart(Unit* unit, ActionParameter* parameter) {
	unit->appendAim(parameter);//TODO parameter do stanu???
}


void PatrolState::onEnd(Unit* unit) {
}

void PatrolState::execute(Unit* unit) {
}
