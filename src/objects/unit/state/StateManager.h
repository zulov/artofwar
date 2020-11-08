#pragma once

#include <magic_enum.hpp>
#include "objects/unit/state/UnitState.h"

struct ActionParameter;
class Unit;
class State;
class Static;
enum class StaticState : char;

class StateManager {
public:
	static bool validateState(int id, UnitState stateTo);
	static bool changeState(Unit* unit, UnitState stateTo, const ActionParameter& actionParameter);
	static bool changeState(Unit* unit, UnitState stateTo);
	static bool checkChangeState(Unit* unit, UnitState stateTo);
	static void execute(Unit* unit, float timeStamp);

	static bool changeState(Static* obj, StaticState stateTo);
	static void executeChange(Static* obj);

	static void init();
	static void dispose();
private:
	StateManager();
	~StateManager();
	static State* getState(Unit* unit);

	State* states[magic_enum::enum_count<UnitState>()]{};

	static StateManager* instance;
};

inline bool isInStates(UnitState state, std::initializer_list<UnitState> states) {
	for (auto s : states) {
		if (state == s) {
			return true;
		}
	}
	return false;
}

inline bool isFree(UnitState state) {
	return isInStates(state, {UnitState::STOP, UnitState::MOVE});
}