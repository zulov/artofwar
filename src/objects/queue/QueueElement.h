#pragma once
#include "ObjectEnums.h"
#include "QueueType.h"

class QueueElement
{
public:
	QueueElement(QueueType _type, short _id, short _maxCapacity, float _initialSecondsToComplete, float _secondsToCompletePerInstance);
	~QueueElement();
	bool checkType(QueueType _type, short _id);
	short add(short value);
	void reduce(short value);
	bool update(float time);
	QueueType getType();
	short getId();
	short getAmount();
	short getMaxCapacity();
	float getProgress();
private:
	QueueType type;
	short id;

	short maxCapacity;
	short amount;

	float secondsToComplete;
	float elapsedSeconds;

	float initialSecondsToComplete;
	float secondsToCompletePerInstance;
};
