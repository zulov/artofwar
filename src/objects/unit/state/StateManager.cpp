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
#include "objects/building/Building.h"
#include "objects/static/StaticStateUtils.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "utils/consts.h"

StateManager* StateManager::instance = nullptr;

StateManager::StateManager() {
	states[cast(UnitState::GO)] = new GoState();
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

	initStates(
		{UnitState::GO, UnitState::MOVE, UnitState::FOLLOW, UnitState::STOP, UnitState::DEAD, UnitState::DISPOSE});

	initOrders({UnitAction::GO, UnitAction::DEAD, UnitAction::STOP, UnitAction::FOLLOW});
}

StateManager::~StateManager() {
	clear_array(states, magic_enum::enum_count<UnitState>());
}

bool StateManager::changeState(Unit* unit, UnitState stateTo) {
	return changeState(unit, stateTo, Consts::EMPTY_ACTION_PARAMETER);
}

bool StateManager::toDefaultState(Unit* unit) {
	return changeState(unit, UnitState::STOP, Consts::EMPTY_ACTION_PARAMETER); //TODO mo¿e lepiej move
}

bool StateManager::canStartState(Unit* unit, UnitState stateTo, const ActionParameter& actionParameter,
                                 State* stateFrom, State* toState) {
	return stateFrom->validateTransition(stateTo)
		&& unit->getLevel()->possibleStates[cast(stateTo)]
		&& toState->canStart(unit, actionParameter);
}

bool StateManager::changeState(Unit* unit, UnitState stateTo, const ActionParameter& actionParameter) {
	State* stateFrom = instance->states[cast(unit->getNextState())];
	State* toState = instance->states[cast(stateTo)];
	if (canStartState(unit, stateTo, actionParameter, stateFrom, toState)) {
		unit->setNextState(stateTo, actionParameter);
		instance->unitHasChanged = true;
		return true;
	}
	Game::getLog()->Write(0, "fail to change state from " +
	                      Urho3D::String(magic_enum::enum_name(unit->getNextState()).data()) + " to " +
	                      Urho3D::String(magic_enum::enum_name(stateTo).data()));

	return false;
}

bool StateManager::canChangeState(Unit* unit, UnitState stateTo) {
	return getState(unit)->validateTransition(stateTo);
}

State* StateManager::getState(Unit* unit) {
	return instance->states[cast(unit->getState())];
}

void StateManager::execute(Unit* unit, float timeStamp) {
	getState(unit)->execute(unit, timeStamp);
}

void StateManager::executeChange(std::vector<Unit*>* units) {
	if (instance->unitHasChanged) {
		instance->unitHasChanged = false;
		for (const auto unit : *units) {
			if (unit->hasStateChangePending()) {
				State* stateFrom = instance->states[cast(unit->getState())];
				State* toState = instance->states[cast(unit->getNextState())];
				//assert(canStartState(unit, unit->getNextState(), unit->getNextActionParameter(), stateFrom, toState));
				if (canStartState(unit, unit->getNextState(), unit->getNextActionParameter(), stateFrom, toState)) {
					stateFrom->onEnd(unit);
					unit->setState(unit->getNextState());
					toState->onStart(unit, unit->getNextActionParameter());
					
				}
				unit->setState(unit->getState());
				unit->getNextActionParameter().resetUsed();
			}
		}
	}
}

bool StateManager::changeState(Static* obj, StaticState stateTo) {
	if (obj->getState() != StaticState::DISPOSE && obj->getState() != stateTo) {
		obj->setNextState(stateTo);
		if (obj->getType() == ObjectType::BUILDING) {
			instance->buildingHasChanged = true;
		} else {
			instance->resourceHasChanged = true;
		}
		return true;
	}
	return false;
}

void StateManager::executeChange(std::vector<Building*>* buildings) {
	if (instance->buildingHasChanged) {
		instance->buildingHasChanged = false;
		for (auto building : *buildings) {
			executeChange(building);
		}
	}
}

void StateManager::executeChange(std::vector<ResourceEntity*>* resources) {
	if (instance->resourceHasChanged) {
		instance->resourceHasChanged = false;
		for (auto resource : *resources) {
			executeChange(resource);
		}
	}
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

void StateManager::initOrders(std::initializer_list<UnitAction> states) const {
	std::vector<char> ids;
	for (auto state : states) {
		ids.push_back(cast(state));
	}
	for (auto level : Game::getDatabase()->getLevels()) {
		if (level) {
			level->ordersIds.insert(level->ordersIds.begin(), ids.begin(), ids.end());
			if (level->canCollect) {
				level->ordersIds.push_back(cast(UnitAction::COLLECT));
			} else {
				level->ordersIds.push_back(cast(UnitAction::DEFEND));
			}
			if (level->canChargeAttack) {
				level->ordersIds.push_back(cast(UnitAction::CHARGE));
			}
			if (level->canCloseAttack || level->canBuildingAttack || level->canRangeAttack || level->canChargeAttack) {
				level->ordersIds.push_back(cast(UnitAction::ATTACK));
			}
			level->ordersIds.shrink_to_fit();
			std::ranges::sort(level->ordersIds);
		}
	}
}

void StateManager::initStates(std::initializer_list<UnitState> states) const {
	constexpr char SIZE = magic_enum::enum_count<UnitState>();
	bool possibleStates[SIZE];
	std::fill_n(possibleStates, SIZE, false);
	for (auto state : states) {
		possibleStates[cast(state)] = true;
	}
	for (auto level : Game::getDatabase()->getLevels()) {
		if (level) {
			std::copy_n(possibleStates, SIZE, level->possibleStates);
			if (level->canCollect) {
				level->possibleStates[cast(UnitState::COLLECT)] = true;
			} else {
				level->possibleStates[cast(UnitState::DEFEND)] = true;
			}
			if (level->canChargeAttack) {
				level->possibleStates[cast(UnitState::CHARGE)] = true;
			}
			if (level->canRangeAttack) {
				level->possibleStates[cast(UnitState::SHOT)] = true;
			}
			if (level->canCloseAttack || level->canBuildingAttack || level->canRangeAttack || level->canChargeAttack) {
				level->possibleStates[cast(UnitState::ATTACK)] = true;
			}
		}
	}
}

void StateManager::setBuildingToDispose(bool value) {
	instance->buildingIsToDispose = value;
}

void StateManager::setResourceToDispose(bool value) {
	instance->resourceIsToDispose = value;
}

bool StateManager::isBuildingToDispose() {
	return instance->buildingIsToDispose;
}

bool StateManager::isResourceToDispose() {
	return instance->resourceIsToDispose;
}
