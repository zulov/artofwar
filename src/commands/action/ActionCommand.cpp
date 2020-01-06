#include "ActionCommand.h"


ActionCommand::ActionCommand(FutureOrder* futureAim): AbstractCommand(0),//TODO BUG PLAYER
	futureAim(futureAim) {
}

ActionCommand::~ActionCommand() = default;

void ActionCommand::execute() {
	bool ifRemove = futureAim->add();
	if (ifRemove) {
		delete futureAim;
	}
}
