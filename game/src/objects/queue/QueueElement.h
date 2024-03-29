#pragma once

enum class QueueActionType : char;
enum class ActionType : char;

class QueueElement {
public:
	QueueElement(QueueActionType type, short id, short maxCapacity, short initialTicksToComplete,
	             short ticksToCompletePerInstance);
	~QueueElement() = default;
	bool checkType(QueueActionType _type, short _id) const;
	short add(short value);
	void reduce(short value);
	bool update();
	
	QueueActionType getType() const { return type; }
	short getId() const { return id; }
	short getAmount() const { return amount; }
	short getMaxCapacity() const { return maxCapacity; }
	float getProgress() const { return ((float)elapsedTicks) / ticksToComplete; }
private:
	QueueActionType type;
	short id;

	short maxCapacity;
	short amount;

	short ticksToComplete;
	short elapsedTicks;

	short ticksToCompletePerInstance;
};
