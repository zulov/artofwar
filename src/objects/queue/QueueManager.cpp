#include "QueueManager.h"
#include "Game.h"
#include "QueueElement.h"
#include "database/DatabaseCache.h"
#include "defines.h"
#include "objects/ActionType.h"
#include "utils.h"
#include <algorithm>


QueueManager::QueueManager(short maxCapacity):maxCapacity(maxCapacity) {
	queue.reserve(DEFAULT_VECTOR_SIZE);
}


QueueManager::~QueueManager() {
	clear_vector(queue);
}

void QueueManager::add(short value, ActionType type, short id, short localMaxCapacity) {
	for (auto& i : queue) {
		if (i->checkType(type, id)) {
			value = i->add(value);
		}
	}
	while (value > 0) {
		float secToInstance = getSecPerInstance(type, id, 0);
		float initialSecondsToComplete = getSecToComplete(type, id, 0);
		auto element = new QueueElement(type, id, std::min(maxCapacity, localMaxCapacity), initialSecondsToComplete,
		                                         secToInstance);
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

		if (element->update(time)) {
			//TODO memoryleak
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

float QueueManager::getSecToComplete(ActionType type, short id, int level) {//TODO performance przerobic na tablice
	switch (type) {
	case ActionType::UNIT_CREATE:
		return 5;
	case ActionType::BUILDING_CREATE:
		return 10;
	case ActionType::UNIT_LEVEL:
		{
		auto dbLevel = Game::getDatabaseCache()->getUnitLevel(id, level).value();
		return dbLevel->upgradeSpeed;
		}
	case ActionType::BUILDING_LEVEL:
		return 10;
	default:
		return 1;
	}
}

float QueueManager::getSecPerInstance(ActionType type, short id, int level) {//TODO performance przerobic na tablice
	switch (type) {
	case ActionType::UNIT_CREATE:
		return 0.5;
	case ActionType::BUILDING_CREATE:
		return 0;
	case ActionType::UNIT_LEVEL:
		return 0;
	case ActionType::BUILDING_LEVEL:
		return 0;
	default:
		return 0;
	}
}
