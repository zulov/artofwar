#include "QueueManager.h"

#include "QueueActionType.h"
#include "QueueElement.h"
#include "utils/DeleteUtils.h"

QueueManager::~QueueManager() { clear_vector(queue); }

void QueueManager::add(QueueActionType type, unsigned short id, unsigned short levelId, short number,
					   unsigned short durationOverride) {
	for (auto i : queue) {
		if (i->checkType(type, id, levelId)) {
			number = i->add(number);
		}
	}

	while (number > 0) {
		unsigned char maxCap = type == QueueActionType::UNIT_CREATE ? maxUnitsGroup : 1;
		auto element = new QueueElement(type, id, levelId, maxCap, durationOverride);
		number = element->add(number);
		queue.push_back(element);
	}
}

QueueElement* QueueManager::update() {
	for (auto i = 0; i < queue.size();) {
		if (queue.at(i)->getAmount() <= 0) {
			delete queue.at(i);
			queue.erase(queue.begin() + i); // BUG chyba iterowanie i usuwanie
		} else {
			++i;
		}
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

short QueueManager::getSize() const { return queue.size(); }

QueueElement* QueueManager::getAt(short i) const { return queue.at(i); }

QueueElement* QueueManager::first() const { return queue.at(0); }

bool QueueManager::contains(QueueActionType type, unsigned short id) const {
	for (const auto* element : queue) {
		if (element->getType() == type && element->getId() == id) return true;
	}
	return false;
}

void QueueManager::changeMaxUnitsGroupSize(unsigned char maxUnitsGroupSize) { maxUnitsGroup = maxUnitsGroupSize; }

void QueueManager::restore(QueueActionType type, unsigned short id, unsigned short levelId, unsigned short amount,
						   unsigned short elapsedTicks, unsigned short durationOverride) {
	const auto maxCapacity = type == QueueActionType::UNIT_CREATE ? maxUnitsGroup : 1;
	auto* element = new QueueElement(type, id, levelId, maxCapacity, durationOverride);
	element->add(static_cast<short>(amount));
	element->restore(elapsedTicks);
	queue.push_back(element);
}
