#include "FollowState.h"


FollowState::FollowState() {
	transitions.insert(UnitStateType::DEAD);
}

FollowState::~FollowState() {
}

void FollowState::onStart(Unit* unit){
}

void FollowState::onEnd(Unit* unit) {
}

void FollowState::execute(Unit* unit) {
}
