#pragma once

#include <initializer_list>
#include <magic_enum.hpp>
#include <vector>

#include "objects/unit/order/enums/UnitAction.h"
#include "objects/unit/state/UnitState.h"

class ResourceEntity;
class Building;
struct ActionParameter;
class Unit;
class State;
class Static;
enum class StaticState : char;

class StateManager {
public:
	static bool changeState(Unit* unit, UnitState stateTo, const ActionParameter& actionParameter);
	static bool changeState(Unit* unit, UnitState stateTo);
	static bool toDefaultState(Unit* unit);
	
	static bool canStartState(Unit* unit, UnitState stateTo, const ActionParameter& actionParameter, State* stateFrom,
	                          State* toState);
	static bool canChangeState(Unit* unit, UnitState stateTo);
	static void execute(Unit* unit, float timeStamp);
	static void executeChange(const std::vector<Unit*>* units);
	static void reset();

	static bool changeState(Static* obj, StaticState stateTo);
	
	static void executeChange(Static* obj);
	static void executeChange(std::vector<Building*>* buildings);
	static void executeChange(std::vector<ResourceEntity*>* resources);

	static bool isUnitToDispose();
	static bool isBuildingToDispose();
	static bool isResourceToDispose();

	static void setUnitDead();
	static void setBuildingDead();
	static void setResourceDead();

	static void setUnitToDispose();
	static void setBuildingToDispose();
	static void setResourceToDispose();

	static bool isUnitUnalived();
	static bool isBuildingUnalived();
	static bool isResourceUnalived();

	static bool isUnitDead();
	static bool isBuildingDead();
	static bool isResourceDead();

	static void init();
	static void dispose();

private:
	void initOrders(std::initializer_list<UnitAction> states) const; //TODO move to level
	void initStates(std::initializer_list<UnitState> states) const; //TODO move to level

	StateManager();
	~StateManager();
	static State* getState(Unit* unit);

	State* states[magic_enum::enum_count<UnitState>()]{};

	static StateManager* instance;

	bool unitStateChangePending = false;
	bool buildingStateChangePending = false;
	bool resourceStateChangePending = false;

	bool unitIsInDeadState = false;
	bool buildingIsInDeadState = false;
	bool resourceIsInDeadState = false;

	bool unitIsInDisposeState = false;
	bool buildingIsInDisposeState = false;
	bool resourceIsInDisposeState = false;
};

inline bool isInStates(UnitState state, std::initializer_list<UnitState> states) {
	for (auto s : states) {
		if (state == s) {
			return true;
		}
	}
	return false;
}

inline bool isInFreeState(UnitState state) {
	return isInStates(state, {UnitState::STOP, UnitState::MOVE});
}
