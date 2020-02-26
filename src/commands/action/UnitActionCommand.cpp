#include "UnitActionCommand.h"


UnitActionCommand::UnitActionCommand(FutureOrder* futureAim, char player): AbstractCommand(player),
                                                                   futureAim(futureAim) {
}

UnitActionCommand::~UnitActionCommand() {
	if (futureAim) {
		int a = 5;
	}
}

void UnitActionCommand::execute() {
	bool ifRemove = futureAim->add();
	if (ifRemove) {
		delete futureAim;
		futureAim = nullptr;
	}
}
