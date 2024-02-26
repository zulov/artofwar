#include "UnitActionCommand.h"
#include "objects/unit/order/UnitOrder.h"


UnitActionCommand::UnitActionCommand(UnitOrder* order)
	: order(order) {}

void UnitActionCommand::execute() {
	const bool ifRemove = order->add();
	if (ifRemove) {
		delete order;
		order = nullptr;
	}
}
