#pragma once
#include "Static.h"
#include "StaticState.h"


void startState(StaticState state, Static* obj) {
	obj->setState(state);
	switch (state) {
	case StaticState::FREE:
		StateManager::changeState(obj, StaticState::ALIVE);
		break;
	case StaticState::DEAD:
		StateManager::changeState(obj, StaticState::DISPOSE);
		break;
	case StaticState::DISPOSE: break;
	default: ;
	}
}

void endState(StaticState state, Static* obj) {
}
