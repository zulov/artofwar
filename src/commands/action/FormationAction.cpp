#include "FormationAction.h"
#include "Game.h"
#include "simulation/env/Enviroment.h"


FormationAction::FormationAction(Formation* formation, OrderType action, Vector2* parameter, bool append)
	: ActionCommand(action, nullptr, parameter, append) {
	this->formation = formation;
}

FormationAction::~FormationAction() {
}

void FormationAction::addTargetAim(Vector2* to, bool append) {
	auto opt = formation->getLeader();
	if (opt.has_value()) {
		auto physical = opt.value();

		ActionParameter parameter = getTargetAim(physical->getBucketIndex(-1), *to, append);
		short id = static_cast<short>(action);
		physical->action(id, parameter);
	}

	Game::get()->getEnviroment()->invalidateCache();
}

void FormationAction::addChargeAim(Vector2* charge, bool append) {
}

void FormationAction::addFollowAim(Physical* toFollow, bool append) {

}
