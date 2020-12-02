#include "StateManager.h"

#include <Urho3D\IO\Log.h>
#include "AttackState.h"
#include "ChargeState.h"
#include "CollectState.h"
#include "DeadState.h"
#include "DefendState.h"
#include "DisposeState.h"
#include "FollowState.h"
#include "GoState.h"
#include "MoveState.h"
#include "ShotState.h"
#include "StopState.h"
#include "database/DatabaseCache.h"
#include "objects/static/StaticStateUtils.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "utils/consts.h"

StateManager* StateManager::instance = nullptr;

StateManager::StateManager() {
	states[cast(UnitState::GO_TO)] = new GoState();
	states[cast(UnitState::STOP)] = new StopState();
	states[cast(UnitState::CHARGE)] = new ChargeState();
	states[cast(UnitState::ATTACK)] = new AttackState();
	states[cast(UnitState::DEFEND)] = new DefendState();
	states[cast(UnitState::FOLLOW)] = new FollowState();
	states[cast(UnitState::COLLECT)] = new CollectState();
	states[cast(UnitState::MOVE)] = new MoveState();
	states[cast(UnitState::SHOT)] = new ShotState();
	states[cast(UnitState::DEAD)] = new DeadState();
	states[cast(UnitState::DISPOSE)] = new DisposeState();
	//TODO improvment uzyc cancollect itp
	std::vector<char> orderToState[magic_enum::enum_count<UnitAction>()];
	orderToState[cast(UnitAction::GO)] = {cast(UnitState::GO_TO), cast(UnitState::MOVE)};
	orderToState[cast(UnitAction::ATTACK)] = {cast(UnitState::ATTACK), cast(UnitState::SHOT)};
	orderToState[cast(UnitAction::DEAD)] = {cast(UnitState::DEAD), cast(UnitState::DISPOSE)};
	orderToState[cast(UnitAction::STOP)] = {cast(UnitState::STOP)};
	orderToState[cast(UnitAction::CHARGE)] = {cast(UnitState::CHARGE)};
	orderToState[cast(UnitAction::DEFEND)] = {cast(UnitState::DEFEND)};
	orderToState[cast(UnitAction::FOLLOW)] = {cast(UnitState::FOLLOW)};
	orderToState[cast(UnitAction::COLLECT)] = {cast(UnitState::COLLECT)};

	for (auto unit : Game::getDatabase()->getUnits()) {
		if (unit) {
			auto orders = unit->orders;
			//test //TTO cos zle
			std::fill_n(unit->possibleStates, magic_enum::enum_count<UnitState>(), false);
			for (auto order : orders) {
				for (auto state : orderToState[order->id]) {
					unit->possibleStates[state] = true;
				}
			}
			unit->possibleStates[cast(UnitState::DEAD)] = true; //Always can die
			unit->possibleStates[cast(UnitState::DISPOSE)] = true; //Always can die
		}
	}
}


StateManager::~StateManager() {
	clear_array(states, magic_enum::enum_count<UnitState>());
}

bool StateManager::validateState(int id, UnitState stateTo) {
	return Game::getDatabase()->getUnit(id)->possibleStates[cast(stateTo)];
}

bool StateManager::changeState(Unit* unit, UnitState stateTo) {
	return changeState(unit, stateTo, Consts::EMPTY_ACTION_PARAMETER);
}

bool StateManager::changeState(Unit* unit, UnitState stateTo, const ActionParameter& actionParameter) {
	State* stateFrom = instance->states[cast(unit->getState())];
	if (stateFrom->validateTransition(stateTo)
		&& validateState(unit->getId(), stateTo)
		&& instance->states[cast(stateTo)]->canStart(unit, actionParameter)) {
		stateFrom->onEnd(unit);
		unit->setState(stateTo);
		instance->states[cast(stateTo)]->onStart(unit, actionParameter);
		return true;
	}
	Game::getLog()->Write(0, "fail to change state from " +
	                      Urho3D::String(magic_enum::enum_name(unit->getState()).data()) + " to " +
	                      Urho3D::String(magic_enum::enum_name(stateTo).data()));

	unit->setState(UnitState::MOVE);
	instance->states[cast(UnitState::MOVE)]->onStart(unit, actionParameter);

	return false;
}

bool StateManager::checkChangeState(Unit* unit, UnitState stateTo) {
	return getState(unit)->validateTransition(stateTo);
}

State* StateManager::getState(Unit* unit) {
	return instance->states[cast(unit->getState())];
}

void StateManager::execute(Unit* unit, float timeStamp) {
	getState(unit)->execute(unit, timeStamp);
}

bool StateManager::changeState(Static* obj, StaticState stateTo) {
	if (obj->getState() != stateTo) {
		obj->setNextState(stateTo);
		return true;
	}
	return false;
}

void StateManager::executeChange(Static* obj) {
	if (obj->getState() != obj->getNextState()) {
		endState(obj->getState(), obj);
		startState(obj->getNextState(), obj);
	}
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
