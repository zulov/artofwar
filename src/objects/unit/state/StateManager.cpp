#include "StateManager.h"
#include "defines.h"
#include "GoState.h"
#include "StopState.h"
#include "ChargeState.h"
#include "AttackState.h"
#include "PatrolState.h"
#include "DeadState.h"
#include "DefendState.h"
#include "UnitStateType.h"
#include "MoveState.h"
#include "FollowState.h"
#include "../Unit.h"

StateManager::StateManager() {
	states = new State*[STATE_SIZE];
	states[static_cast<int>(UnitStateType::GO)] = new GoState();
	states[static_cast<int>(UnitStateType::STOP)] = new StopState();
	states[static_cast<int>(UnitStateType::CHARAGE)] = new ChargeState();
	states[static_cast<int>(UnitStateType::ATTACK)] = new AttackState();
	states[static_cast<int>(UnitStateType::PATROL)] = new PatrolState();
	states[static_cast<int>(UnitStateType::DEAD)] = new DeadState();
	states[static_cast<int>(UnitStateType::DEFEND)] = new DefendState();
	states[static_cast<int>(UnitStateType::MOVE)] = new MoveState();
	states[static_cast<int>(UnitStateType::FOLLOW)] = new FollowState();
}


StateManager::~StateManager() {
}

void StateManager::changeState(Unit* unit, UnitStateType stateTo) {
	State * stateFrom = states[static_cast<int>(unit->getState())];
	if (stateFrom->validateTransition(stateTo)) {
		stateFrom->onEnd(unit);
		unit->setState(stateTo);
		states[static_cast<int>(stateTo)]->onStart(unit);
	}
}

bool StateManager::checkChangeState(Unit* unit, UnitStateType stateTo) {
	State * stateFrom = states[static_cast<int>(unit->getState())];
	if (stateFrom->validateTransition(stateTo)) {
		return true;
	}
	return false;
}
