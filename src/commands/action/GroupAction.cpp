#include "GroupAction.h"


GroupAction::GroupAction(std::vector<Physical*>* entities, OrderType action, Vector2* parameter, bool append)
	: ActionCommand(action, nullptr, parameter, append) {
	this->entities = entities;

}

GroupAction::GroupAction(std::vector<Physical*>* entities, OrderType action, Physical* paremeter, bool append)
	: ActionCommand(action, paremeter, nullptr, append) {
	this->entities = entities;
}

GroupAction::~GroupAction() {
}
