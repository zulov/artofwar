#include "ChargeState.h"
#include "../Unit.h"


ChargeState::ChargeState() {
	nextStates[static_cast<char>(UnitStateType::STOP)] = true;
	nextStates[static_cast<char>(UnitStateType::DEFEND)] = true;
	nextStates[static_cast<char>(UnitStateType::DEAD)] = true;
	nextStates[static_cast<char>(UnitStateType::GO)] = true;
	nextStates[static_cast<char>(UnitStateType::PATROL)] = true;
	nextStates[static_cast<char>(UnitStateType::FOLLOW)] = true;
	nextStates[static_cast<char>(UnitStateType::CHARAGE)] = true;
	nextStates[static_cast<char>(UnitStateType::MOVE)] = true;
}


ChargeState::~ChargeState() = default;

void ChargeState::onStart(Unit* unit) {
}

void ChargeState::onStart(Unit* unit, ActionParameter& parameter) {
	unit->addAim(parameter.aim);
	//TODO aim?
	unit->maxSpeed = unit->dbLevel->maxSpeed * 2;
	unit->chargeData->reset();
}

void ChargeState::onEnd(Unit* unit) {
	State::onEnd(unit);
	unit->maxSpeed = unit->dbLevel->maxSpeed;
}

void ChargeState::execute(Unit* unit) {
	State::execute(unit);
	++unit->currentFrameState;
	if (unit->chargeData->updateFrame()) {
		for (auto physical : unit->thingsToInteract) {
			if (unit->getTeam() != physical->getTeam()) {
				auto before = physical->getHealthPercent();
				physical->absorbAttack(10);
				auto after = physical->getHealthPercent();
				if (!unit->chargeData->updateHit(before, after)) {
					StateManager::get()->changeState(unit, UnitStateType::MOVE);
					break;
				}
			}
		}
	} else {
		StateManager::get()->changeState(unit, UnitStateType::MOVE);
	}

}
