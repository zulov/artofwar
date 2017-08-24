#pragma once
#include "ObjectEnums.h"

class QueueElement
{
public:
	QueueElement(ObjectType _type, short _subType, short _maxCapacity);
	~QueueElement();
	bool checkType(ObjectType _type, short _subType);
	short add(short value);
	void remove(short value);
	bool update(float time);
	ObjectType getType();
	short getSubtype();
	short getAmount();
	short getMaxCapacity();
	float getProgress();
private:
	ObjectType type;
	short subType;

	short maxCapacity;
	short amount;

	float secondsToComplete;
	float elapsedSeconds;

	float initialSecondsToComplete = 5;
	float secondsToCompletePerInstance = 0.5f;
};
