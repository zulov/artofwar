#include "FollowState.h"
#include "../Unit.h"


FollowState::FollowState() {
	transitions.insert(UnitStateType::DEAD);
}

FollowState::~FollowState() {
}

void FollowState::onStart(Unit* unit){
}
void FollowState::onStart(Unit* unit, ActionParameter* parameter) {
	unit->followAim(parameter);//TODO parameter do stanu???
}


void FollowState::onEnd(Unit* unit) {
}

void FollowState::execute(Unit* unit) {
}
