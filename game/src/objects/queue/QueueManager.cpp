#include "QueueManager.h"
#include "QueueElement.h"
#include "QueueUtils.h"


QueueManager::QueueManager(unsigned short maxCapacity): maxCapacity(maxCapacity) {
}


QueueManager::~QueueManager() {
	clear_vector(queue);
}

void QueueManager::add(short number, QueueActionType type, short id, unsigned short localMaxCapacity) {
	for (auto i : queue) {
		if (i->checkType(type, id)) {
			number = i->add(number);
		}
	}

	while (number > 0) {
		auto element = new QueueElement(type, id, std::min(maxCapacity, localMaxCapacity),
		                                getSecToComplete(type, id, 0),
		                                getSecPerInstance(type, id, 0));
		number = element->add(number);
		queue.push_back(element);
	}
}

QueueElement* QueueManager::update() {
	for (auto i = 0; i < queue.size(); ++i) {
		if (queue.at(i)->getAmount() <= 0) {
			delete queue.at(i);
			queue.erase(queue.begin() + i);
		}
	}
	if (!queue.empty()) {
		auto element = *queue.begin();

		if (element->update()) {
			queue.erase(queue.begin());
			return element;
		}
	}
	return nullptr;
}

short QueueManager::getSize() const {
	return queue.size();
}

QueueElement* QueueManager::getAt(short i) {
	return queue.at(i);
}
