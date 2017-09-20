#include "QueueManager.h"
#include "defines.h"
#include <iostream>
#include "utils.h"


QueueManager::QueueManager(short _maxCapacity) {
	queue = new std::vector<QueueElement*>();
	queue->reserve(DEFAULT_VECTOR_SIZE);

	maxCapacity = _maxCapacity;
}


QueueManager::~QueueManager() {
	clear_vector(queue);
}

void QueueManager::add(short value, ObjectType type, short id) {
	for (int i = 0; i < queue->size(); ++i) {
		if (queue->at(i)->checkType(type, id)) {
			value = queue->at(i)->add(value);
		}
	}
	while (value > 0) {
		QueueElement* element = new QueueElement(type, id, maxCapacity);
		value = element->add(value);
		queue->push_back(element);
	}
}

QueueElement* QueueManager::update(float time) {
	if (queue->size() > 0) {
		QueueElement* element = queue->at(0);
		if (element->update(time)) {
			queue->erase(queue->begin());
			return element;
		}
	}
}

short QueueManager::getSize() {
	return queue->size();
}

QueueElement* QueueManager::getAt(short i) {
	return queue->at(i);
}