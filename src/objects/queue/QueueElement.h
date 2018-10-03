#pragma once

enum class MenuAction : char;

class QueueElement
{
public:
	QueueElement(MenuAction type, short id, short maxCapacity, float initialSecondsToComplete,
	             float secondsToCompletePerInstance);
	~QueueElement();
	bool checkType(MenuAction _type, short _id);
	short add(short value);
	void reduce(short value);
	bool update(float time);
	MenuAction getType();
	short getId();
	short getAmount();
	short getMaxCapacity();
	float getProgress();
private:
	MenuAction type;
	short id;

	short maxCapacity;
	short amount;

	float secondsToComplete;
	float elapsedSeconds;

	float initialSecondsToComplete;
	float secondsToCompletePerInstance;
};
