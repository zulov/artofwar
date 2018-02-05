#include "StateManager.h"
#include "../Unit.h"
#include "AttackState.h"
#include "ChargeState.h"
#include "CollectState.h"
#include "DeadState.h"
#include "DefendState.h"
#include "FollowState.h"
#include "Game.h"
#include "GoState.h"
#include "MoveState.h"
#include "PatrolState.h"
#include "StopState.h"
#include "DisposeState.h"
#include "UnitStateType.h"
#include "database/DatabaseCache.h"


StateManager* StateManager::instance = nullptr;

StateManager::StateManager() {
	states[static_cast<int>(UnitStateType::GO)] = new GoState();
	states[static_cast<int>(UnitStateType::STOP)] = new StopState();
	states[static_cast<int>(UnitStateType::CHARAGE)] = new ChargeState();
	states[static_cast<int>(UnitStateType::ATTACK)] = new AttackState();
	states[static_cast<int>(UnitStateType::PATROL)] = new PatrolState();
	states[static_cast<int>(UnitStateType::DEAD)] = new DeadState();
	states[static_cast<int>(UnitStateType::DEFEND)] = new DefendState();
	states[static_cast<int>(UnitStateType::FOLLOW)] = new FollowState();
	states[static_cast<int>(UnitStateType::COLLECT)] = new CollectState();
	states[static_cast<int>(UnitStateType::MOVE)] = new MoveState();
	states[static_cast<int>(UnitStateType::DISPOSE)] = new DisposeState();
	for (int i = 0; i < UNITS_NUMBER_DB; ++i) {
		std::vector<db_order*>* orders = Game::get()->getDatabaseCache()->getOrdersForUnit(i);
		for (auto order : *orders) {
			ordersToUnit[i].insert(UnitStateType(order->id));
		}
		ordersToUnit[i].insert(UnitStateType::MOVE);//TODO nie³¹dny hardcode bo stanie nie sa 1:1 z orderami
	}
}


StateManager::~StateManager() {
	for (auto state : states) {
		delete state;
	}
}

void StateManager::changeState(Unit* unit, UnitStateType stateTo) {
	State* stateFrom = states[static_cast<int>(unit->getState())];
	if (stateFrom->validateTransition(stateTo)) {
		stateFrom->onEnd(unit);
		unit->setState(stateTo);
		states[static_cast<int>(stateTo)]->onStart(unit);
	}
}

bool StateManager::validateState(int id, UnitStateType stateTo) {
	return ordersToUnit[id].find(stateTo) != ordersToUnit[id].end();
}

void StateManager::changeState(Unit* unit, UnitStateType stateTo, ActionParameter& actionParameter) {
	State* stateFrom = states[static_cast<int>(unit->getState())];
	if (stateFrom->validateTransition(stateTo) && validateState(unit->getDbID(), stateTo)) {
		stateFrom->onEnd(unit);
		unit->setState(stateTo);
		states[static_cast<int>(stateTo)]->onStart(unit, actionParameter);
	}
}

bool StateManager::checkChangeState(Unit* unit, UnitStateType stateTo) {
	State* stateFrom = states[static_cast<int>(unit->getState())];
	return stateFrom->validateTransition(stateTo);
}

void StateManager::execute(Unit* unit) {
	State* state = states[static_cast<int>(unit->getState())];
	state->execute(unit);
}

StateManager* StateManager::get() {
	return instance;
}

void StateManager::init() {
	if (instance == nullptr) {
		instance = new StateManager();
	}
}

void StateManager::dispose() {
	delete instance;
	instance = nullptr;
}
