#include "IndividualAction.h"


IndividualAction::IndividualAction(Physical* entity, OrderType action, Physical* paremeter, bool append)
	: ActionCommand(action, paremeter, nullptr, append) {
	this->entity = entity;
}

IndividualAction::~IndividualAction() {
}
