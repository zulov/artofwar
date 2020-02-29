#include "UnitActionCommand.h"


UnitActionCommand::UnitActionCommand(FutureOrder* futureAim, char player): AbstractCommand(player),
                                                                           futureAim(futureAim) {
}

UnitActionCommand::~UnitActionCommand() = default;

void UnitActionCommand::execute() {
	bool ifRemove = futureAim->add();
	if (ifRemove) {
		delete futureAim;
		futureAim = nullptr;
	}
}
