#include "ActionCommand.h"


ActionCommand::ActionCommand(FutureOrder* futureAim, bool append): futureAim(futureAim),
	append(append) {
}

ActionCommand::~ActionCommand() {
}

void ActionCommand::execute() {
	futureAim->add(append);
}
