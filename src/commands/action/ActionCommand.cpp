#include "ActionCommand.h"


ActionCommand::ActionCommand(FutureOrder* futureAim):
	futureAim(futureAim) {
}

ActionCommand::~ActionCommand() = default;

void ActionCommand::execute() {
	bool ifRemove = futureAim->add();
	if (ifRemove) {
		delete futureAim;
	}
}
