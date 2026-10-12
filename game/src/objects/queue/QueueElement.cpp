#include "QueueElement.h"
#include "QueueActionType.h"
#include "QueueUtils.h"

#include <algorithm>

QueueElement::QueueElement(QueueActionType type, unsigned short id, unsigned char maxCapacity, unsigned short durationTicks) :
		type(type), maxCapacity(maxCapacity), id(id), amount(0),
		ticksToComplete(durationTicks ? durationTicks : getSecToComplete(type, id)), elapsedTicks(0),
		baseTicksToComplete(ticksToComplete), ticksToCompletePerInstance(getSecPerInstance(type, id)) {}

bool QueueElement::checkType(QueueActionType _type, unsigned short _id) const { return type == _type && id == _id; }

short QueueElement::add(short value) {
	if (value <= 0) return 0;
	const short accepted = std::min<short>(value, static_cast<short>(maxCapacity - amount));
	const short previousAmount = amount;
	amount += accepted;
	const short extraInstances = std::max<short>(0, amount - 1) - std::max<short>(0, previousAmount - 1);
	ticksToComplete += extraInstances * ticksToCompletePerInstance;
	return value - accepted;
}

void QueueElement::reduce(short value) {
	if (value <= 0) return;
	amount = std::max<short>(0, amount - value);
	ticksToComplete = baseTicksToComplete + std::max<short>(0, amount - 1) * ticksToCompletePerInstance;
}

bool QueueElement::update() {
	++elapsedTicks;
	return elapsedTicks >= ticksToComplete;
}

void QueueElement::restore(unsigned short savedAmount, unsigned short savedElapsedTicks, unsigned short savedDurationTicks) {
	amount = savedAmount;
	elapsedTicks = savedElapsedTicks;
	if (savedDurationTicks != 0) ticksToComplete = savedDurationTicks;
	const auto extraTicks = static_cast<unsigned short>(std::max<short>(0, amount - 1) * ticksToCompletePerInstance);
	baseTicksToComplete = ticksToComplete >= extraTicks ? ticksToComplete - extraTicks : 0;
}
