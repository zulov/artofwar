#include "QueueElement.h"

QueueElement::QueueElement(ActionType type, short id, short maxCapacity, float initialSecondsToComplete,
                           float secondsToCompletePerInstance): type(type), id(id), maxCapacity(maxCapacity), amount(0),
	secondsToComplete(initialSecondsToComplete), elapsedSeconds(0),
	initialSecondsToComplete(initialSecondsToComplete),
	secondsToCompletePerInstance(secondsToCompletePerInstance) {
}

QueueElement::~QueueElement() = default;

bool QueueElement::checkType(ActionType _type, short _id) const {
	return type == _type && id == _id;
}

short QueueElement::add(short value) {
	short rest;
	if (maxCapacity <= amount + value) {
		rest = amount + value - maxCapacity;
		amount = maxCapacity;
	} else {
		amount += value;
		secondsToComplete += value * secondsToCompletePerInstance;
		rest = 0;
	}
	return rest;
}

void QueueElement::reduce(short value) {
	amount -= value;
}

bool QueueElement::update(float time) {
	elapsedSeconds += time;
	return elapsedSeconds >= secondsToComplete;
}

ActionType QueueElement::getType() const {
	return type;
}

short QueueElement::getId() const {
	return id;
}

short QueueElement::getAmount() const {
	return amount;
}

short QueueElement::getMaxCapacity() const {
	return maxCapacity;
}

float QueueElement::getProgress() const {
	return elapsedSeconds / secondsToComplete;
}
