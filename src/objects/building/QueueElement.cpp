#include "QueueElement.h"

QueueElement::QueueElement(ObjectType _type, short _subType, short _maxCapacity) {
	elapsedSeconds = 0;
	amount = 0;
	secondsToComplete = initialSecondsToComplete;
	type = _type;
	subType = _subType;
	maxCapacity = _maxCapacity;
}

QueueElement::~QueueElement() = default;

bool QueueElement::checkType(ObjectType _type, short _subType) {
	return type == _type && subType == _subType;
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

ObjectType QueueElement::getType() {
	return type;
}

short QueueElement::getSubtype() {
	return subType;
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
