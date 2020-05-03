#include "UnitActionCommand.h"

#include "objects/unit/order/UnitOrder.h"


UnitActionCommand::UnitActionCommand(UnitOrder* order, char player): AbstractCommand(player),
                                                                     order(order) {
}

UnitActionCommand::~UnitActionCommand() = default;

void UnitActionCommand::execute() {
	bool ifRemove = order->add();
	if (ifRemove) {
		delete order;
		order = nullptr;
	}
}
