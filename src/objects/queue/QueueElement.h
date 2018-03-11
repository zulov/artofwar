#pragma once
#include "objects/ActionType.h"

class QueueElement
{
public:
	QueueElement(ActionType _type, short _id, short _maxCapacity, float _initialSecondsToComplete, float _secondsToCompletePerInstance);
	~QueueElement();
	bool checkType(ActionType _type, short _id);
	short add(short value);
	void reduce(short value);
	bool update(float time);
	ActionType getType();
	short getId();
	short getAmount();
	short getMaxCapacity();
	float getProgress();
private:
	ActionType type;
	short id;

	short maxCapacity;
	short amount;

	float secondsToComplete;
	float elapsedSeconds;

	float initialSecondsToComplete;
	float secondsToCompletePerInstance;
};
