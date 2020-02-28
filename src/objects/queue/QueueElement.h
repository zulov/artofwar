#pragma once

enum class QueueActionType : char;
enum class ActionType : char;

class QueueElement {
public:
	QueueElement(QueueActionType type, short id, short maxCapacity, float initialSecondsToComplete,
	             float secondsToCompletePerInstance);
	~QueueElement();
	bool checkType(QueueActionType _type, short _id) const;
	short add(short value);
	void reduce(short value);
	bool update(float time);
	QueueActionType getType() const;
	short getId() const;
	short getAmount() const;
	short getMaxCapacity() const;
	float getProgress() const;
private:
	QueueActionType type;
	short id;

	short maxCapacity;
	short amount;

	float secondsToComplete;
	float elapsedSeconds;

	float initialSecondsToComplete;
	float secondsToCompletePerInstance;
};
