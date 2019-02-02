#include "GroupOrder.h"
#include "FormationOrder.h"
#include "Game.h"
#include "objects/unit/ActionParameter.h"
#include "simulation/formation/FormationManager.h"


GroupOrder::GroupOrder(std::vector<Physical*>* entities, UnitOrder action, const Urho3D::Vector2& vector,
                       const Physical* physical, MenuAction menuAction, bool append):
	FutureOrder(action, append, vector, physical), entities(entities), menuAction(menuAction) {
}

GroupOrder::~GroupOrder() = default;

bool GroupOrder::add() {
	switch (menuAction) {

	case MenuAction::UNIT_CREATE:
		simpleAction(ActionParameter(menuAction));
		break;
	case MenuAction::ORDER:
		execute();
		break;
	case MenuAction::NONE:
	case MenuAction::UNIT_LEVEL:
	case MenuAction::UNIT_UPGRADE:
	case MenuAction::BUILDING:
	case MenuAction::BUILDING_LEVEL:
	case MenuAction::FORMATION:
	case MenuAction::RESOURCE:
		{
		int a = 5;
		}
		break;
	default: ;
	}

	return true;
}

void GroupOrder::addTargetAim() {
	auto opt = Game::getFormationManager()->createFormation(entities);
	if (opt.has_value()) {
		opt.value()->addOrder(new FormationOrder(opt.value(), action, vector, nullptr, append));
	}
}

void GroupOrder::addFollowAim() {
	transformToFormationOrder();
}

void GroupOrder::addChargeAim() {
	transformToFormationOrder();
}

void GroupOrder::addAttackAim() {
	transformToFormationOrder();
}

void GroupOrder::addDefendAim() {
	simpleAction(ActionParameter());
}

void GroupOrder::addDeadAim() {
	simpleAction(ActionParameter());
}

void GroupOrder::simpleAction(ActionParameter parameter) {
	for (auto entity : *entities) {
		entity->action(static_cast<char>(FutureOrder::action), parameter);
	}
}

void GroupOrder::transformToFormationOrder() const {
	auto opt = Game::getFormationManager()->createFormation(entities);
	if (opt.has_value()) {
		opt.value()->addOrder(new FormationOrder(opt.value(), action, vector, toUse, append));
	}
}
