#include "UnitActionCommand.h"

#include <cassert>

#include "scene/load/RuntimeSaveData.h"
#include "objects/unit/order/UnitOrder.h"


UnitActionCommand::UnitActionCommand(UnitOrder* order)
	: order(order) {}

void UnitActionCommand::execute() {
	assert(!order->expired());
	const bool ifRemove = order->add();
	if (ifRemove) {
		delete order;
		order = nullptr;
	}
}

PendingCommandSaveData UnitActionCommand::saveState(unsigned short order) const {
	return this->order->saveState(order);
}
