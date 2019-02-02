#include "QueueElement.h"

QueueElement::QueueElement(ActionType type, short id, short maxCapacity, float initialSecondsToComplete,
                           float secondsToCompletePerInstance): elapsedSeconds(0), amount(0), type(type), id(id),
	maxCapacity(maxCapacity), secondsToComplete(initialSecondsToComplete),
	initialSecondsToComplete(initialSecondsToComplete),
	secondsToCompletePerInstance(secondsToCompletePerInstance) {

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
