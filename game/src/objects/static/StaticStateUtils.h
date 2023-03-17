#pragma once
#include "Static.h"
#include "StaticState.h"


inline void startState(StaticState state, Static* obj) {
	obj->setState(state);
	switch (state) {
	case StaticState::FREE:
		StateManager::changeState(obj, StaticState::ALIVE);
		break;
	case StaticState::DEAD:
		StateManager::changeState(obj, StaticState::DISPOSE);
		break;
	case StaticState::DISPOSE:
		if (obj->getType() == ObjectType::BUILDING) {
			StateManager::setBuildingToDispose(true);
		} else {
			StateManager::setResourceToDispose(true);
		}
		break;
	default: ;
	}
}

inline void endState(StaticState state, Static* obj) {}
