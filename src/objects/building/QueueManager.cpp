#include "QueueManager.h"
#include "defines.h"
#include <iostream>
#include "utils.h"


QueueManager::QueueManager(short _maxCapacity) {
	queue.reserve(DEFAULT_VECTOR_SIZE);

	maxCapacity = _maxCapacity;
}


QueueManager::~QueueManager() {
	clear_vector(queue);
}

void QueueManager::add(short value, ObjectType type, short id) {
	for (auto & i : queue) {
		if (i->checkType(type, id)) {
			value = i->add(value);
		}
	}
	while (value > 0) {
		QueueElement* element = new QueueElement(type, id, maxCapacity);
		value = element->add(value);
		queue.push_back(element);
	}
}

QueueElement* QueueManager::update(float time) {
	for (int i = 0; i < queue.size(); ++i) {
		if (queue.at(i)->getAmount() <= 0) {
			queue.erase(queue.begin() + i);
		}
	}
	if (!queue.empty()) {
		QueueElement* element = queue.at(0);

		if (element->update(time)) {//TODO memoryleak
			queue.erase(queue.begin());
			return element;
		}

	}
	return nullptr;
}

short QueueManager::getSize() {
	return queue.size();
}

QueueElement* QueueManager::getAt(short i) {
	return queue.at(i);
}
