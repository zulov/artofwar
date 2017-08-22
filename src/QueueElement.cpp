#include "QueueElement.h"

QueueElement::QueueElement(ObjectType _type, short _subType, short _maxCapacity) {
	elapsedSeconds = 0;
	amount = 0;
	secondsToComplete = initialSecondsToComplete;
	type = _type;
	subType = _subType;
	maxCapacity = _maxCapacity;
}

QueueElement::~QueueElement() {
}

bool QueueElement::checkType(ObjectType _type, short _subType) {
	if (type == _type && subType == _subType) {
		return true;
	}
	return false;
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

void QueueElement::remove(short value) {
	amount -= value;
}

bool QueueElement::update(float time) {
	elapsedSeconds += time;
	if (elapsedSeconds >= secondsToComplete) {
		return true;
	}
	return false;
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
