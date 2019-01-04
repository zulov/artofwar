#include "ActionCommand.h"


ActionCommand::ActionCommand(FutureOrder* futureAim, bool append): futureAim(futureAim),
	append(append) {
}

ActionCommand::~ActionCommand() {
	delete futureAim;
}

void ActionCommand::execute() {
	addAim();
}
