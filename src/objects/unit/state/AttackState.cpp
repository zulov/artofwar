#include "AttackState.h"
#include "../Unit.h"


AttackState::AttackState() {
	transitions.insert(UnitStateType::STOP);
	transitions.insert(UnitStateType::DEFEND);
	transitions.insert(UnitStateType::DEAD);
	//transitions.insert(UnitStateType::ATTACK);
	transitions.insert(UnitStateType::GO);
	transitions.insert(UnitStateType::FOLLOW);
	transitions.insert(UnitStateType::CHARAGE);
}


AttackState::~AttackState() = default;

void AttackState::onStart(Unit* unit) {
	*unit->velocity = Urho3D::Vector3::ZERO;
}

void AttackState::onStart(Unit* unit, ActionParameter& parameter) {
}

void AttackState::onEnd(Unit* unit) {
	unit->enemyToAttack = nullptr;
}

void AttackState::execute(Unit* unit) {
	(*unit->velocity) = Urho3D::Vector3::ZERO;
	unit->enemyToAttack->absorbAttack(unit->attackCoef);
}
