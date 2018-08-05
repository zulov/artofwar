#pragma once
#include "StaticState.h"
#include "Static.h"

void startState(StaticState state, Static* obj) {
	switch (state) {

	case StaticState::DEAD:
		obj->nextState = StaticState::DISPOSE;
		break;
	case StaticState::DISPOSE: break;
	default: ;
	}
}

void endState(StaticState state, Static* obj) {

}
