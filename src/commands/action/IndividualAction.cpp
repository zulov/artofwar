#include "IndividualAction.h"
#include "Game.h"
#include "objects/unit/Unit.h"
#include "simulation/env/Enviroment.h"


IndividualAction::IndividualAction(Physical* entity, UnitOrder action, const Physical* paremeter, bool append)
	: ActionCommand(action, paremeter, nullptr, append), entity(entity) {
}

IndividualAction::IndividualAction(Physical* entity, UnitOrder action, const Urho3D::Vector2& vector, bool append) :
	ActionCommand(action, nullptr, new Urho3D::Vector2(vector), append), entity(entity) {
}

IndividualAction::~IndividualAction() = default;

void IndividualAction::addTargetAim(Urho3D::Vector2* to, bool append) {
	entity->action(static_cast<char>(action), getTargetAim(entity->getMainCell(), *to));
	static_cast<Unit*>(entity)->resetFormation();

	Game::getEnviroment()->invalidateCache();
}

void IndividualAction::addChargeAim(Urho3D::Vector2* charge, bool append) {
	entity->action(static_cast<char>(action), getChargeAim(charge));
}

void IndividualAction::addFollowAim(const Physical* toFollow, bool append) {
	entity->action(static_cast<char>(action), 
		  getFollowAim(entity->getMainCell(),
		                                 toFollow->getPosToFollow(entity->getPosition()),
		                                 toFollow));

}
