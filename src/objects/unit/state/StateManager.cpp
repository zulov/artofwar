#include "StateManager.h"

#include <magic_enum.hpp>
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
#include "../Unit.h"
#include "../../static/Static.h"
#include "database/DatabaseCache.h"
#include "objects/static/StaticStateUtils.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "utils/consts.h"

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
	std::vector<char> orderToState[magic_enum::enum_count<UnitAction>()];
	orderToState[static_cast<char>(UnitAction::GO)] =
	{
		static_cast<char>(UnitState::GO_TO),
		static_cast<char>(UnitState::MOVE)
	};
	orderToState[static_cast<char>(UnitAction::ATTACK)] = {
		static_cast<char>(UnitState::ATTACK),
		static_cast<char>(UnitState::SHOT)
	};
	orderToState[static_cast<char>(UnitAction::DEAD)] = {
		static_cast<char>(UnitState::DEAD),
		static_cast<char>(UnitState::DISPOSE)
	};
	orderToState[static_cast<char>(UnitAction::STOP)] = {static_cast<char>(UnitState::STOP)};
	orderToState[static_cast<char>(UnitAction::CHARGE)] = {static_cast<char>(UnitState::CHARGE)};
	orderToState[static_cast<char>(UnitAction::DEFEND)] = {static_cast<char>(UnitState::DEFEND)};
	orderToState[static_cast<char>(UnitAction::FOLLOW)] = {static_cast<char>(UnitState::FOLLOW)};
	orderToState[static_cast<char>(UnitAction::COLLECT)] = {static_cast<char>(UnitState::COLLECT)};

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
			unit->possibleStates[static_cast<char>(UnitState::DEAD)] = true; //Always can die
			unit->possibleStates[static_cast<char>(UnitState::DISPOSE)] = true; //Always can die
		}
	}
}


StateManager::~StateManager() {
	clear_array(states,magic_enum::enum_count<UnitState>());
}

bool StateManager::validateState(int id, UnitState stateTo) {
	return Game::getDatabase()->getUnit(id)->possibleStates[static_cast<char>(stateTo)];
}

bool StateManager::changeState(Unit* unit, UnitState stateTo) {
	return changeState(unit, stateTo, Consts::EMPTY_ACTION_PARAMETER);
}

bool StateManager::changeState(Unit* unit, UnitState stateTo, const ActionParameter& actionParameter) {
	State* stateFrom = instance->states[static_cast<int>(unit->getState())];
	if (stateFrom->validateTransition(stateTo)
		&& validateState(unit->getId(), stateTo)
		&& instance->states[static_cast<int>(stateTo)]->canStart(unit, actionParameter)) {
		stateFrom->onEnd(unit);
		unit->setState(stateTo);
		instance->states[static_cast<int>(stateTo)]->onStart(unit, actionParameter);
		return true;
	}
	Game::getLog()->Write(0, "fail to change state from " +
	                      Urho3D::String(magic_enum::enum_name(unit->getState()).data()) + " to " +
	                      Urho3D::String(magic_enum::enum_name(stateTo).data()));

	unit->setState(UnitState::MOVE);
	instance->states[static_cast<int>(UnitState::MOVE)]->onStart(unit, actionParameter);

	return false;
}

bool StateManager::checkChangeState(Unit* unit, UnitState stateTo) {
	return getState(unit)->validateTransition(stateTo);
}

State* StateManager::getState(Unit* unit) {
	return instance->states[static_cast<int>(unit->getState())];
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
