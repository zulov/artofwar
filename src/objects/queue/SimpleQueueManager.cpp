#include "SimpleQueueManager.h"
#include "QueueElement.h"
#include "QueueUtils.h"

SimpleQueueManager::~SimpleQueueManager() {
	delete element;
}

void SimpleQueueManager::add(short number, QueueActionType type, short id, unsigned short localMaxCapacity) {
	if (!element) {
		element = new QueueElement(type, id, 1, getSecToComplete(type, id, 0),
		                           getSecPerInstance(type, id, 0));
		element->add(number);
	}
}

QueueElement* SimpleQueueManager::update() {
	if (element) {
		if (element->getAmount() <= 0) {
			delete element;
			element = nullptr;
		}
	}
	if (element) {
		if (element->update()) {
			auto el = element;
			element = nullptr;
			return el;
		}
	}
	return nullptr;
}

short SimpleQueueManager::getSize() const {
	return element == nullptr ? 0 : 1;
}

QueueElement* SimpleQueueManager::getAt(short i) {
	if (getSize() == 1 && i == 0) {
		return element;
	}
	return nullptr;
}
