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
#include "UnitOrder.h"
#include "UnitState.h"
#include "database/DatabaseCache.h"


StateManager* StateManager::instance = nullptr;

StateManager::StateManager() {
	states[static_cast<char>(UnitState::GO_TO)] = new GoState();
	states[static_cast<char>(UnitState::STOP)] = new StopState();
	states[static_cast<char>(UnitState::CHARGE)] = new ChargeState();
	states[static_cast<char>(UnitState::ATTACK)] = new AttackState();
	states[static_cast<char>(UnitState::DEFEND)] = new DefendState();
	states[static_cast<char>(UnitState::FOLLOW)] = new FollowState();
	states[static_cast<char>(UnitState::COLLECT)] = new CollectState();
	states[static_cast<char>(UnitState::MOVE)] = new MoveState();
	states[static_cast<char>(UnitState::SHOT)] = new ShotState();
	states[static_cast<char>(UnitState::DEAD)] = new DeadState();
	states[static_cast<char>(UnitState::DISPOSE)] = new DisposeState();

	orderToState[static_cast<char>(UnitOrder::GO)] =
	{
		static_cast<char>(UnitState::GO_TO),
		static_cast<char>(UnitState::MOVE)
	};
	orderToState[static_cast<char>(UnitOrder::ATTACK)] = {
		static_cast<char>(UnitState::ATTACK),
		static_cast<char>(UnitState::SHOT)
	};
	orderToState[static_cast<char>(UnitOrder::DEAD)] = {
		static_cast<char>(UnitState::DEAD), 
		static_cast<char>(UnitState::DISPOSE)
	};
	orderToState[static_cast<char>(UnitOrder::STOP)] = {static_cast<char>(UnitState::STOP)};
	orderToState[static_cast<char>(UnitOrder::CHARGE)] = {static_cast<char>(UnitState::CHARGE)};
	orderToState[static_cast<char>(UnitOrder::DEFEND)] = {static_cast<char>(UnitState::DEFEND)};
	orderToState[static_cast<char>(UnitOrder::FOLLOW)] = {static_cast<char>(UnitState::FOLLOW)};
	orderToState[static_cast<char>(UnitOrder::COLLECT)] = {static_cast<char>(UnitState::COLLECT)};

	for (int i = 0; i < UNITS_NUMBER_DB; ++i) {
		std::vector<db_order*>* orders = Game::getDatabaseCache()->getOrdersForUnit(i);
		fill_n(ordersToUnit[i], STATE_SIZE, false);
		for (auto order : *orders) {
			for (auto state : orderToState[order->id]) {
				ordersToUnit[i][state] = true;
			}
		}
	}
}


StateManager::~StateManager() {
	for (auto state : states) {
		delete state;
	}
}

bool StateManager::validateState(int id, UnitState stateTo) {
	return instance->ordersToUnit[id][static_cast<char>(stateTo)];
}

void StateManager::changeState(Unit* unit, UnitState stateTo, ActionParameter& actionParameter) {
	State* stateFrom = instance->states[static_cast<int>(unit->getState())];
	if (stateFrom->validateTransition(stateTo) && validateState(unit->getDbID(), stateTo)) {
		stateFrom->onEnd(unit);
		unit->setState(stateTo);
		instance->states[static_cast<int>(stateTo)]->onStart(unit, actionParameter);
	}
}

bool StateManager::checkChangeState(Unit* unit, UnitState stateTo) {
	State* stateFrom = instance->states[static_cast<int>(unit->getState())];
	return stateFrom->validateTransition(stateTo);
}

void StateManager::execute(Unit* unit) {
	State* state = instance->states[static_cast<int>(unit->getState())];
	state->execute(unit);
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
