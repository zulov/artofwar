#include "StateManager.h"
#include "../Unit.h"
#include "AttackState.h"
#include "ChargeState.h"
#include "CollectState.h"
#include "DeadState.h"
#include "DefendState.h"
#include "DisposeState.h"
#include "FollowState.h"
#include "Game.h"
#include "GoState.h"
#include "MoveState.h"
#include "ShotState.h"
#include "StopState.h"
#include "UnitState.h"
#include "database/DatabaseCache.h"


StateManager* StateManager::instance = nullptr;

StateManager::StateManager() {
	states[static_cast<int>(UnitState::GO)] = new GoState();
	states[static_cast<int>(UnitState::STOP)] = new StopState();
	states[static_cast<int>(UnitState::CHARAGE)] = new ChargeState();
	states[static_cast<int>(UnitState::ATTACK)] = new AttackState();
	states[static_cast<int>(UnitState::DEAD)] = new DeadState();
	states[static_cast<int>(UnitState::DEFEND)] = new DefendState();
	states[static_cast<int>(UnitState::FOLLOW)] = new FollowState();
	states[static_cast<int>(UnitState::COLLECT)] = new CollectState();
	states[static_cast<int>(UnitState::MOVE)] = new MoveState();
	states[static_cast<int>(UnitState::DISPOSE)] = new DisposeState();
	states[static_cast<int>(UnitState::SHOT)] = new ShotState();
	for (int i = 0; i < UNITS_NUMBER_DB; ++i) {
		std::vector<db_order*>* orders = Game::get()->getDatabaseCache()->getOrdersForUnit(i);
		fill_n(ordersToUnit[i], STATE_SIZE, false);
		for (auto order : *orders) {
			ordersToUnit[i][order->id] = true;
		}
		ordersToUnit[i][static_cast<char>(UnitState::MOVE)] = true;
		//TODO nie³¹dny hardcode bo stanie nie sa 1:1 z orderami
	}
}


StateManager::~StateManager() {
	for (auto state : states) {
		delete state;
	}
}

void StateManager::changeState(Unit* unit, UnitState stateTo) {
	State* stateFrom = states[static_cast<int>(unit->getState())];
	if (stateFrom->validateTransition(stateTo)) {
		stateFrom->onEnd(unit);
		unit->setState(stateTo);
		states[static_cast<int>(stateTo)]->onStart(unit);
	}
}

bool StateManager::validateState(int id, UnitState stateTo) {
	return ordersToUnit[id][static_cast<char>(stateTo)];
}

void StateManager::changeState(Unit* unit, UnitState stateTo, ActionParameter& actionParameter) {
	State* stateFrom = states[static_cast<int>(unit->getState())];
	if (stateFrom->validateTransition(stateTo) && validateState(unit->getDbID(), stateTo)) {
		stateFrom->onEnd(unit);
		unit->setState(stateTo);
		states[static_cast<int>(stateTo)]->onStart(unit, actionParameter);
	}
}

bool StateManager::checkChangeState(Unit* unit, UnitState stateTo) {
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
