#pragma once

#include <cstddef>

enum class QueueActionType : unsigned char;
enum class ActionType : unsigned char;

class QueueElement {
public:
	QueueElement(QueueActionType type, unsigned short id, unsigned char maxCapacity, unsigned short durationOverride = 0);
	~QueueElement() = default;
	bool checkType(QueueActionType _type, unsigned short _id) const;
	short add(short value);
	void reduce(short value);
	bool update();
	
	QueueActionType getType() const { return type; }
	unsigned short getId() const { return id; }
	unsigned short getAmount() const { return amount; }
	unsigned char getMaxCapacity() const { return maxCapacity; }
	unsigned short getTicksToComplete() const { return ticksToComplete; }
	unsigned short getElapsedTicks() const { return elapsedTicks; }
	float getProgress() const { return ticksToComplete == 0 ? 1.f : ((float)elapsedTicks) / ticksToComplete; }
	void restore(unsigned short savedElapsedTicks);

private:
	QueueActionType type;
	unsigned char maxCapacity;

	unsigned short id;
	unsigned short amount;

	unsigned short ticksToComplete;
	unsigned short elapsedTicks;

	unsigned short ticksToCompletePerInstance;
};
