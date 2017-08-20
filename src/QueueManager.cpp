#include "QueueManager.h"
#include "defines.h"
#include <iostream>


QueueManager::QueueManager() {
	queue = new std::vector<QueueElement*>();
	queue->reserve(DEFAULT_VECTOR_SIZE);
	ended = new std::vector<QueueElement*>();
	ended->reserve(DEFAULT_VECTOR_SIZE);
}


QueueManager::~QueueManager() {
}

void QueueManager::add(short value, ObjectType type, short id) {
	for (int i = 0; i < queue->size(); ++i) {
		if (queue->at(i)->checkType(type, id)) {
			value = queue->at(i)->add(value);
		}
	}
	while (value > 0) {
		QueueElement* element = new QueueElement(type, id);
		value = element->add(value);
		queue->push_back(element);
	}
}

std::vector<QueueElement*>* QueueManager::update(float time) {
	ended->clear();
	for (int i = queue->size() - 1; i >= 0; --i) {
		QueueElement* element = queue->at(i);
		if (element->update(time)) {
			ended->push_back(element);
			queue->erase(queue->begin() + i);
		}
	}
	return ended;
}
