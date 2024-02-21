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
enum class ObjectType : char;

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
	static const std::vector<Unit*>& getDeadUnits() { return instance->deadUnits; }
	static const std::vector<Building*>& getDeadBuildings() { return instance->deadBuildings; }
	static const std::vector<ResourceEntity*>& getDeadResources() { return instance->deadResources; }

	static bool changeState(Static* obj, StaticState stateTo);

	static void executeChange(Static* obj);
	static void executeChange(std::vector<Building*>* buildings);
	static void executeChange(std::vector<ResourceEntity*>* resources);
	static void startState(Static* obj);

	static bool isUnitToDispose();
	static bool isBuildingToDispose();
	static bool isResourceToDispose();

	static bool isSthToDispose();

	static bool isUnitDead();
	static bool isBuildingDead();
	static bool isResourceDead();

	static void init();
	static void dispose();

private:
	void initOrders(std::initializer_list<UnitAction> states) const; //TODO move to level
	void initStates(std::initializer_list<UnitState> states) const; //TODO move to level


	static void setStaticDead(Static* object);
	static void setStaticToDispose(ObjectType object);

	StateManager();
	~StateManager();
	static State* getState(Unit* unit);

	State* states[magic_enum::enum_count<UnitState>()]{};

	static StateManager* instance;

	bool unitStateChangePending = false;
	bool buildingStateChangePending = false;
	bool resourceStateChangePending = false;

	bool unitIsInDisposeState = false;
	bool buildingIsInDisposeState = false;
	bool resourceIsInDisposeState = false;

	std::vector<Unit*> deadUnits;
	std::vector<Building*> deadBuildings;
	std::vector<ResourceEntity*> deadResources;
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
	//return isInStates(state, {UnitState::STOP, UnitState::MOVE});
	return state == UnitState::STOP || state == UnitState::MOVE;
}
