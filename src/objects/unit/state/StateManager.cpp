#include "StateManager.h"
#include "defines.h"
#include "GoState.h"
#include "StopState.h"
#include "ChargeState.h"
#include "AttackState.h"
#include "PatrolState.h"
#include "DeadState.h"
#include "DefendState.h"


StateManager::StateManager() {
	states = new State*[STATE_SIZE];

	states[0] = new GoState();
	states[1] = new StopState();
	states[2] = new ChargeState();
	states[3] = new AttackState();
	states[4] = new PatrolState();
	states[5] = new DeadState();
	states[6] = new DefendState();
	
}


StateManager::~StateManager() {
}
