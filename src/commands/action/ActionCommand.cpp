#include "ActionCommand.h"


ActionCommand::ActionCommand(FutureOrder* futureAim, bool append):
	futureAim(futureAim), append(append) {
}

ActionCommand::~ActionCommand() = default;

void ActionCommand::execute() {
	bool ifRemove = futureAim->add(append);
	if (ifRemove) {
		delete futureAim;
	}
}
