#pragma once
#include "ObjectEnums.h"
#include "QueueType.h"

class QueueElement
{
public:
	QueueElement(QueueType _type, short _subType, short _maxCapacity);
	~QueueElement();
	bool checkType(QueueType _type, short _subType);
	short add(short value);
	void reduce(short value);
	bool update(float time);
	QueueType getType();
	short getSubtype();
	short getAmount();
	short getMaxCapacity();
	float getProgress();
private:
	QueueType type;
	short subType;

	short maxCapacity;
	short amount;

	float secondsToComplete;
	float elapsedSeconds;

	float initialSecondsToComplete = 5;
	float secondsToCompletePerInstance = 0.5f;
};
