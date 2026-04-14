#include "QueueManager.h"

#include "QueueActionType.h"
#include "QueueElement.h"
#include "utils/DeleteUtils.h"


QueueManager::~QueueManager() {
	clear_vector(queue);
}


void QueueManager::add(QueueActionType type, short id, short levelId, short number) {
	for (auto i : queue) {
		if (i->checkType(type, id, levelId)) {
			number = i->add(number);
		}
	}

	while (number > 0) {
		unsigned char maxCap = type == QueueActionType::UNIT_CREATE ? maxUnitsGroup : 1;
		auto element = new QueueElement(type, id, levelId, maxCap);
		number = element->add(number);
		queue.push_back(element);
	}
}

QueueElement* QueueManager::update() {
	for (auto i = 0; i < queue.size();) {
		if (queue.at(i)->getAmount() <= 0) {
			delete queue.at(i);
			queue.erase(queue.begin() + i); //BUG chyba iterowanie i usuwanie 
		} else { ++i; }
	}
	if (!queue.empty()) {
		const auto element = *queue.begin();

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

QueueElement* QueueManager::getAt(short i) const {
	return queue.at(i);
}

QueueElement* QueueManager::first() const {
	return queue.at(0);
}

void QueueManager::changeMaxUnitsGroupSize(unsigned char maxUnitsGroupSize) {
	maxUnitsGroup = maxUnitsGroupSize;
}
