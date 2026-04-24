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
#include "objects/unit/order/UnitConst.h"
#include "player/ai/ActionCenter.h"
#include "utils/consts.h"
#include "utils/OtherUtils.h"

StateManager* StateManager::instance = nullptr;

StateManager::StateManager() {
	states[castC(UnitState::STOP)] = new StopState();
	states[castC(UnitState::MOVE)] = new MoveState();
	states[castC(UnitState::GO)] = new GoState();
	states[castC(UnitState::FOLLOW)] = new FollowState();

	states[castC(UnitState::ATTACK)] = new AttackState();
	states[castC(UnitState::SHOT)] = new ShotState();
	states[castC(UnitState::CHARGE)] = new ChargeState();
	states[castC(UnitState::DEFEND)] = new DefendState();
	states[castC(UnitState::COLLECT)] = new CollectState();

	states[castC(UnitState::DEAD)] = new DeadState();
	states[castC(UnitState::DISPOSE)] = new DisposeState();

	initStates();

	initOrders();
}

StateManager::~StateManager() {
	clear_array(states, magic_enum::enum_count<UnitState>());
}

bool StateManager::changeState(Unit* unit, UnitState stateTo) {
	return changeState(unit, stateTo, Consts::EMPTY_ACTION_PARAMETER);
}

bool StateManager::toDefaultState(Unit* unit) {
	return changeState(unit, UnitState::STOP, Consts::EMPTY_ACTION_PARAMETER); //TODO mo�e lepiej move
}

bool StateManager::changeState(Unit* unit, UnitState stateTo, const ActionParameter& actionParameter) {
	State* stateFrom = instance->states[castC(unit->getNextState())];
	State* toState = instance->states[castC(stateTo)];
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
		&& unit->getDb()->possibleStates[castC(stateTo)]
		&& toState->canStart(unit, actionParameter);
}

bool StateManager::canChangeState(Unit* unit, UnitState stateTo) {
	return getState(unit)->validateTransition(stateTo);
}

State* StateManager::getState(Unit* unit) {
	return instance->states[castC(unit->getState())];
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
				State* stateFrom = instance->states[castC(unit->getState())];
				State* toState = instance->states[castC(nextState)];

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
		if (obj->getType() == ObjectType::BUILDING) {//TODO to mozre dac to building->updateAi
			auto building = (Building*)obj;
			auto [data, level] = building->getData();

			auto res = data->toResource;
			if (res >= 0 && level->spawnResourceRange<=0) {
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
					auto costs = building->getDb();
					if (costs->maxFromWoodOrStone > 0) {
						short id = costs->moreWoodThanStone ? 6 : 5; //TODO hardcoded

						Game::getActionCenter()->addResource(id, building->getMainGridIndex(),
						                                     costs->maxFromWoodOrStone * 0.2f);
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

void StateManager::initOrders() const {
	std::vector<char> ids;

	ids.push_back(castC(UnitAction::GO));
	ids.push_back(castC(UnitAction::DEAD));
	ids.push_back(castC(UnitAction::STOP));
	ids.push_back(castC(UnitAction::FOLLOW));

	for (auto unit : Game::getDatabase()->getUnits()) {
		if (unit) {
			unit->ordersIds.insert(unit->ordersIds.begin(), ids.begin(), ids.end());
			if (unit->typeWorker) {
				unit->ordersIds.push_back(castC(UnitAction::COLLECT));
			} else {
				unit->ordersIds.push_back(castC(UnitAction::DEFEND));
			}
			if (unit->typeCavalry) {
				unit->ordersIds.push_back(castC(UnitAction::CHARGE));
			}
			if (unit->typeMelee || unit->typeRange || unit->typeCavalry) {
				unit->ordersIds.push_back(castC(UnitAction::ATTACK));
			}
			unit->ordersIds.shrink_to_fit();
			std::ranges::sort(unit->ordersIds);
		}
	}
}

void StateManager::initStates() const {
	constexpr char SIZE = magic_enum::enum_count<UnitState>();
	bool defaultPossibleStates[SIZE];
	std::fill_n(defaultPossibleStates, SIZE, false);

	defaultPossibleStates[castC(UnitState::GO)] = true;
	defaultPossibleStates[castC(UnitState::MOVE)] = true;
	defaultPossibleStates[castC(UnitState::FOLLOW)] = true;
	defaultPossibleStates[castC(UnitState::STOP)] = true;
	defaultPossibleStates[castC(UnitState::DEAD)] = true;
	defaultPossibleStates[castC(UnitState::DISPOSE)] = true;

	for (const auto unit : Game::getDatabase()->getUnits()) {
		if (unit) {
			std::copy_n(defaultPossibleStates, SIZE, unit->possibleStates);
			if (unit->typeWorker) {
				unit->possibleStates[castC(UnitState::COLLECT)] = true;
			} else {
				unit->possibleStates[castC(UnitState::DEFEND)] = true;
			}
			if (unit->typeCavalry) {
				unit->possibleStates[castC(UnitState::CHARGE)] = true;
			}
			if (unit->typeRange) {
				unit->possibleStates[castC(UnitState::SHOT)] = true;
			}
			if (unit->typeMelee || unit->typeRange || unit->typeCavalry) {
				unit->possibleStates[castC(UnitState::ATTACK)] = true;
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
	return instance->unitIsInDisposeState || instance->buildingIsInDisposeState || instance->resourceIsInDisposeState;
}
