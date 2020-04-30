#include "UnitActionCommand.h"


UnitActionCommand::UnitActionCommand(UnitOrder* order, char player): AbstractCommand(player),
                                                                           order(order) {
}

UnitActionCommand::~UnitActionCommand() = default;

void UnitActionCommand::execute() {
	bool ifRemove = futureAim->add();
	if (ifRemove) {
		delete futureAim;
		futureAim = nullptr;
	}
}
