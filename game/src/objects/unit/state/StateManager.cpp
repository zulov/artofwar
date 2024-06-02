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
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "player/ai/ActionCenter.h"
#include "utils/consts.h"
#include "utils/OtherUtils.h"

StateManager* StateManager::instance = nullptr;

StateManager::StateManager() {
	states[cast(UnitState::STOP)] = new StopState();
	states[cast(UnitState::MOVE)] = new MoveState();
	states[cast(UnitState::GO)] = new GoState();
	states[cast(UnitState::FOLLOW)] = new FollowState();

	states[cast(UnitState::ATTACK)] = new AttackState();
	states[cast(UnitState::SHOT)] = new ShotState();
	states[cast(UnitState::CHARGE)] = new ChargeState();
	states[cast(UnitState::DEFEND)] = new DefendState();
	states[cast(UnitState::COLLECT)] = new CollectState();

	states[cast(UnitState::DEAD)] = new DeadState();
	states[cast(UnitState::DISPOSE)] = new DisposeState();

	initStates(
	           {
		           UnitState::GO, UnitState::MOVE, UnitState::FOLLOW, UnitState::STOP, UnitState::DEAD,
		           UnitState::DISPOSE
	           });

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

bool StateManager::changeState(Unit* unit, UnitState stateTo, const ActionParameter& actionParameter) {
	State* stateFrom = instance->states[cast(unit->getNextState())];
	State* toState = instance->states[cast(stateTo)];
	if (canStartState(unit, stateTo, actionParameter, stateFrom, toState)) {
		unit->setNextState(stateTo, actionParameter);
		instance->unitStateChangePending = true;
		return true;
	}
	Game::getLog()->Write(0, "fail to change state from " +
	                      Urho3D::String(magic_enum::enum_name(unit->getNextState()).data()) + " to " +
	                      Urho3D::String(magic_enum::enum_name(stateTo).data()));

	return false;
}

bool StateManager::canStartState(Unit* unit, UnitState stateTo, const ActionParameter& actionParameter,
                                 State* stateFrom, State* toState) {
	return stateFrom->validateTransition(stateTo)
		&& unit->getDb()->possibleStates[cast(stateTo)]
		&& toState->canStart(unit, actionParameter);
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

void StateManager::executeChange(const std::vector<Unit*>* units) {
	if (instance->unitStateChangePending) {
		instance->unitStateChangePending = false;
		for (const auto unit : *units) {
			if (unit->hasStateChangePending()) {
				const auto nextState = unit->getNextState();
				State* stateFrom = instance->states[cast(unit->getState())];
				State* toState = instance->states[cast(nextState)];

				if (canStartState(unit, nextState, unit->getNextActionParameter(), stateFrom, toState)) {
					stateFrom->onEnd(unit);
					unit->setState(nextState);
					if (nextState == UnitState::DEAD) {
						instance->deadUnits.push_back(unit);
					} else if (nextState == UnitState::DISPOSE) {
						instance->unitIsInDisposeState = true;
					}
					toState->onStart(unit, unit->getNextActionParameter());
					unit->getNextActionParameter().resetUsed();
				} else {
					const bool mayHaveAim = nextState == UnitState::GO || nextState == UnitState::CHARGE || nextState ==
						UnitState::FOLLOW;
					unit->getNextActionParameter().reset(mayHaveAim);
				}
				unit->resetStateChangePending();
			}
		}
	}
}

void StateManager::reset() {
	instance->deadUnits.clear();
	instance->deadBuildings.clear();
	instance->deadResources.clear();

	instance->unitIsInDisposeState = false;
	instance->buildingIsInDisposeState = false;
	instance->resourceIsInDisposeState = false;
}

bool StateManager::changeState(Static* obj, StaticState stateTo) {
	if (obj->getState() != StaticState::DISPOSE && obj->getState() != stateTo) {
		obj->setNextState(stateTo);
		if (obj->getType() == ObjectType::BUILDING) {
			instance->buildingStateChangePending = true;
		} else {
			instance->resourceStateChangePending = true;
		}
		return true;
	}
	return false;
}

void StateManager::executeChange(std::vector<Building*>* buildings) {
	if (instance->buildingStateChangePending) {
		instance->buildingStateChangePending = false;
		for (const auto building : *buildings) {
			//TODO perf kolekcja tylko zmienionych i iterowanie tylko po nich
			executeChange(building);
		}
	}
}

void StateManager::executeChange(std::vector<ResourceEntity*>* resources) {
	if (instance->resourceStateChangePending) {
		instance->resourceStateChangePending = false;
		for (const auto resource : *resources) {
			//TODO perf kolekcja tylko zmienionych i iterowanie tylko po nich
			executeChange(resource);
		}
	}
}

void StateManager::startState(Static* obj) {
	obj->setState(obj->getNextState());
	switch (obj->getNextState()) {
	case StaticState::FREE:
		changeState(obj, StaticState::ALIVE);
		break;
	case StaticState::ALIVE:
		if (obj->getType() == ObjectType::BUILDING) {
			auto building = (Building*)obj;
			auto res = building->getDb()->toResource;
			if (res >= 0) {
				changeState(obj, StaticState::DEAD);
			}
		}
		break;
	case StaticState::DEAD:
		changeState(obj, StaticState::DISPOSE);
		setStaticDead(obj);
		break;
	case StaticState::DISPOSE:
		if (obj->getType() == ObjectType::BUILDING) {
			auto building = (Building*)obj;
			if (obj->getHp() <= 0.f) {
				if (building->getDb()->ruinable) {
					auto costs = building->getDb()->costs;
					if (costs->anyWoodOrStone) {
						short id = costs->moreWoodThanStone ? 6 : 5;
						float amount = costs->moreWoodThanStone ? costs->wood : costs->stone;

						Game::getActionCenter()->addResource(id, building->getMainGridIndex(), amount * 0.2f);
					}
				}
			} else {
				auto res = building->getDb()->toResource;
				if (res >= 0) {
					Game::getActionCenter()->addResource(res, building->getMainGridIndex());
				}
			}
		}
		setStaticToDispose(obj->getType());
		break;
	default: ;
	}
}

void StateManager::executeChange(Static* obj) {
	if (obj->getState() != obj->getNextState()) {
		//endState(obj->getState(), obj);

		startState(obj);
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
	for (const auto state : states) {
		ids.push_back(cast(state));
	}

	for (auto unit : Game::getDatabase()->getUnits()) {
		if (unit) {
			unit->ordersIds.insert(unit->ordersIds.begin(), ids.begin(), ids.end());
			if (unit->typeWorker) {
				unit->ordersIds.push_back(cast(UnitAction::COLLECT));
			} else {
				unit->ordersIds.push_back(cast(UnitAction::DEFEND));
			}
			if (unit->typeCalvary) {
				unit->ordersIds.push_back(cast(UnitAction::CHARGE));
			}
			if (unit->typeMelee || unit->typeRange || unit->typeCalvary) {
				unit->ordersIds.push_back(cast(UnitAction::ATTACK));
			}
			unit->ordersIds.shrink_to_fit();
			std::ranges::sort(unit->ordersIds);
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
	for (const auto unit : Game::getDatabase()->getUnits()) {
		if (unit) {
			std::copy_n(possibleStates, SIZE, unit->possibleStates);
			if (unit->typeWorker) {
				unit->possibleStates[cast(UnitState::COLLECT)] = true;
			} else {
				unit->possibleStates[cast(UnitState::DEFEND)] = true;
			}
			if (unit->typeCalvary) {
				unit->possibleStates[cast(UnitState::CHARGE)] = true;
			}
			if (unit->typeRange) {
				unit->possibleStates[cast(UnitState::SHOT)] = true;
			}
			if (unit->typeMelee || unit->typeRange || unit->typeCalvary) {
				unit->possibleStates[cast(UnitState::ATTACK)] = true;
			}
		}
	}
}

void StateManager::setStaticDead(Static* object) {
	if (object->getType() == ObjectType::BUILDING) {
		instance->deadBuildings.push_back(static_cast<Building*>(object));
	} else {
		instance->deadResources.push_back(static_cast<ResourceEntity*>(object));
	}
}

void StateManager::setStaticToDispose(ObjectType object) {
	if (object == ObjectType::BUILDING) {
		instance->buildingIsInDisposeState = true;
	} else {
		instance->resourceIsInDisposeState = true;
	}
}

bool StateManager::isUnitDead() {
	return !instance->deadUnits.empty();
}

bool StateManager::isBuildingDead() {
	return !instance->deadBuildings.empty();
}

bool StateManager::isResourceDead() {
	return !instance->deadResources.empty();
}

bool StateManager::isUnitToDispose() {
	return instance->unitIsInDisposeState;
}

bool StateManager::isBuildingToDispose() {
	return instance->buildingIsInDisposeState;
}

bool StateManager::isResourceToDispose() {
	return instance->resourceIsInDisposeState;
}

bool StateManager::isSthToDispose() {
	return instance->unitIsInDisposeState || instance->resourceIsInDisposeState || instance->resourceIsInDisposeState;
}
