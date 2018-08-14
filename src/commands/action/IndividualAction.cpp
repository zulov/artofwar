#include "IndividualAction.h"
#include "Game.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/Unit.h"
#include "simulation/env/Enviroment.h"


IndividualAction::IndividualAction(Physical* entity, UnitOrder action, const Physical* paremeter, bool append)
	: ActionCommand(action, paremeter, nullptr, append) {
	this->entity = entity;
}

IndividualAction::IndividualAction(Physical* entity, UnitOrder action, const Urho3D::Vector2& vector, bool append) :
	ActionCommand(action, nullptr, new Urho3D::Vector2(vector), append) {
	this->entity = entity;
}

IndividualAction::~IndividualAction() = default;

void IndividualAction::addTargetAim(Urho3D::Vector2* to, bool append) {
	short id = static_cast<char>(action);

	ActionParameter parameter = getTargetAim(entity->getMainCell(), *to, append);
	entity->action(id, parameter);
	static_cast<Unit*>(entity)->resetFormation();

	Game::getEnviroment()->invalidateCache();
}

void IndividualAction::addChargeAim(Urho3D::Vector2* charge, bool append) {
	short id = static_cast<char>(action);

	ActionParameter parameter = getChargeAim(charge, append);
	entity->action(id, parameter);
}

void IndividualAction::addFollowAim(const Physical* toFollow, bool append) {
	short id = static_cast<char>(action);

	ActionParameter parameter = getFollowAim(toFollow, append);
	entity->action(id, parameter);
}
