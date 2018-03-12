#include "QueueElement.h"

QueueElement::QueueElement(ActionType _type, short _id, short _maxCapacity, float _initialSecondsToComplete,
                           float _secondsToCompletePerInstance) {
	elapsedSeconds = 0;
	amount = 0;
	initialSecondsToComplete = _initialSecondsToComplete;
	secondsToCompletePerInstance = _secondsToCompletePerInstance;
	secondsToComplete = initialSecondsToComplete;
	type = _type;
	id = _id;
	maxCapacity = _maxCapacity;
}

QueueElement::~QueueElement() = default;

bool QueueElement::checkType(ActionType _type, short _id) {
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

ActionType QueueElement::getType() {
	return type;
}

short QueueElement::getId() {
	return id;
}

short QueueElement::getAmount() {
	return amount;
}

short QueueElement::getMaxCapacity() {
	return maxCapacity;
}

float QueueElement::getProgress() {
	return elapsedSeconds / secondsToComplete;
}
