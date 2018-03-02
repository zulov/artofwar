#include "QueueElement.h"

QueueElement::QueueElement(QueueType _type, short _subType, short _maxCapacity, float _initialSecondsToComplete,
                           float _secondsToCompletePerInstance) {
	elapsedSeconds = 0;
	amount = 0;
	initialSecondsToComplete = _initialSecondsToComplete;
	secondsToCompletePerInstance = _secondsToCompletePerInstance;
	secondsToComplete = initialSecondsToComplete;
	type = _type;
	id = _subType;
	maxCapacity = _maxCapacity;
}

QueueElement::~QueueElement() = default;

bool QueueElement::checkType(QueueType _type, short _subType) {
	return type == _type && id == _subType;
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

QueueType QueueElement::getType() {
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
