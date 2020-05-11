#include "QueueManager.h"
#include "Game.h"
#include "QueueElement.h"
#include "database/DatabaseCache.h"
#include "utils/defines.h"
#include "QueueActionType.h"
#include "utils/DeleteUtils.h"
#include <algorithm>


QueueManager::QueueManager(short maxCapacity): maxCapacity(maxCapacity) {
	queue.reserve(DEFAULT_VECTOR_SIZE / 2);
}


QueueManager::~QueueManager() {
	clear_vector(queue);
}

void QueueManager::add(short number, QueueActionType type, short id, short localMaxCapacity) {
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

QueueElement* QueueManager::update(float time) {
	for (int i = 0; i < queue.size(); ++i) {
		if (queue.at(i)->getAmount() <= 0) {
			delete queue.at(i);
			queue.erase(queue.begin() + i);
		}
	}
	if (!queue.empty()) {
		QueueElement* element = *queue.begin();

		if (element->update(time)) {
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

float QueueManager::getSecToComplete(QueueActionType type, short id, int level) {
	switch (type) {
	case QueueActionType::UNIT_CREATE:
		return 5;
	case QueueActionType::BUILDING_CREATE:
		return 10;
	case QueueActionType::UNIT_LEVEL:
		return Game::getDatabase()->getUnit(id)->getLevel(level).value()->upgradeSpeed; //TODO BUG value
	case QueueActionType::BUILDING_LEVEL:
		return 10;
	default:
		return 1;
	}
}

float QueueManager::getSecPerInstance(QueueActionType type, short id, int level) {
	//TODO performance przerobic na tablice
	switch (type) {
	case QueueActionType::UNIT_CREATE:
		return 0.5;
	case QueueActionType::BUILDING_CREATE:
	case QueueActionType::UNIT_LEVEL:
	case QueueActionType::BUILDING_LEVEL:
	default:
		return 0;
	}
}
