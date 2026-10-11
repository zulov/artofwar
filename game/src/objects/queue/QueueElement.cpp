#include "QueueElement.h"
#include "QueueActionType.h"
#include "QueueUtils.h"

QueueElement::QueueElement(QueueActionType type, unsigned short id, unsigned char maxCapacity, unsigned short durationOverride) :
		type(type), maxCapacity(maxCapacity), id(id), amount(0),
		ticksToComplete(durationOverride ? durationOverride : getSecToComplete(type, id)), elapsedTicks(0),
		ticksToCompletePerInstance(getSecPerInstance(type, id)) {}

bool QueueElement::checkType(QueueActionType _type, unsigned short _id) const { return type == _type && id == _id; }

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

void QueueElement::restore(unsigned short savedElapsedTicks) {
	elapsedTicks = savedElapsedTicks;
}
