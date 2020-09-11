#include "QueueElement.h"
#include "QueueActionType.h"

QueueElement::QueueElement(QueueActionType type, short id, short maxCapacity, float initialSecondsToComplete,
                           float secondsToCompletePerInstance): type(type), id(id), maxCapacity(maxCapacity), amount(0),
                                                                secondsToComplete(initialSecondsToComplete), elapsedSeconds(0),
                                                                secondsToCompletePerInstance(secondsToCompletePerInstance) {
}

bool QueueElement::checkType(QueueActionType _type, short _id) const {
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
	secondsToComplete -= value * secondsToCompletePerInstance;
}

bool QueueElement::update(float time) {
	elapsedSeconds += time;
	return elapsedSeconds >= secondsToComplete;
}
