#include "AttackState.h"
#include "../Unit.h"
#include <iostream>


AttackState::AttackState() {
	nextStates[static_cast<char>(UnitStateType::STOP)] = true;
	nextStates[static_cast<char>(UnitStateType::DEFEND)] = true;
	nextStates[static_cast<char>(UnitStateType::DEAD)] = true;
	nextStates[static_cast<char>(UnitStateType::GO)] = true;
	nextStates[static_cast<char>(UnitStateType::FOLLOW)] = true;
	nextStates[static_cast<char>(UnitStateType::CHARAGE)] = true;
}


AttackState::~AttackState() = default;

void AttackState::onStart(Unit* unit) {
	unit->velocity = Urho3D::Vector2::ZERO;
	unit->currentFrameState = 0;
}

void AttackState::onStart(Unit* unit, ActionParameter& parameter) {
}

void AttackState::onEnd(Unit* unit) {
	State::onEnd(unit);
	unit->thingsToInteract.clear();
}

void AttackState::execute(Unit* unit) {
	State::execute(unit);
	if (unit->isFirstThingAlive()) {
		unit->velocity = Urho3D::Vector2::ZERO;
		if (fmod(unit->currentFrameState, 1 / unit->attackSpeed) < 1) {
			unit->thingsToInteract[0]->absorbAttack(unit->attackCoef);
		}
		++unit->currentFrameState;
	} else {
		unit->thingsToInteract.clear();
		StateManager::get()->changeState(unit, UnitStateType::STOP);
	}
}
