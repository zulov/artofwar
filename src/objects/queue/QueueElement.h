#pragma once

enum class ActionType : char;

class QueueElement
{
public:
	QueueElement(ActionType type, short id, short maxCapacity, float initialSecondsToComplete,
	             float secondsToCompletePerInstance);
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
