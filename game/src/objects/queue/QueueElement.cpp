#include "QueueElement.h"
#include "QueueActionType.h"
#include "QueueUtils.h"

QueueElement::QueueElement(QueueActionType type, short id, short secondId, unsigned char maxCapacity) :
	type(type),
	maxCapacity(maxCapacity), id(id), levelId(secondId), amount(0),
	ticksToComplete(getSecToComplete(type, id, secondId)), elapsedTicks(0),
	ticksToCompletePerInstance(getSecPerInstance(type, id, secondId)) {}

bool QueueElement::checkType(QueueActionType _type, short _id, short _secondId) const {
	return type == _type && id == _id && _secondId == levelId;
}

short QueueElement::add(short value) {
	short rest;
	if (maxCapacity <= amount + value) {
		rest = amount + value - maxCapacity;
		amount = maxCapacity;
	} else {
		amount += value;
		ticksToComplete += value * ticksToCompletePerInstance;
		rest = 0;
	}
	return rest;
}

void QueueElement::reduce(short value) {
	amount -= value;
	ticksToComplete -= value * ticksToCompletePerInstance;
}

bool QueueElement::update() {
	++elapsedTicks;
	return elapsedTicks >= ticksToComplete;
}
