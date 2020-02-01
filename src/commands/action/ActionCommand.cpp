#include "ActionCommand.h"


ActionCommand::ActionCommand(FutureOrder* futureAim, char player): AbstractCommand(player), //TODO BUG PLAYER
                                                                   futureAim(futureAim) {
}

ActionCommand::~ActionCommand() = default;

void ActionCommand::execute() {
	bool ifRemove = futureAim->add();
	if (ifRemove) {
		delete futureAim;
	}
}
