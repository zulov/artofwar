#include "FormationAction.h"


FormationAction::FormationAction(Formation* formation, OrderType action, Vector2* parameter, bool append)
	: ActionCommand(action, nullptr, parameter, append) {
	this->formation = formation;
}

FormationAction::~FormationAction() {
}
