#pragma once

enum class QueueActionType : unsigned char;
enum class ActionType : unsigned char;

class QueueElement {
public:
	QueueElement(QueueActionType type, short id, short secondId, unsigned char maxCapacity);
	~QueueElement() = default;
	bool checkType(QueueActionType _type, unsigned short _id, unsigned short _secondId) const;
	short add(short value);
	void reduce(short value);
	bool update();
	
	QueueActionType getType() const { return type; }
	short getId() const { return id; }
	short getLevelId() const { return levelId; }
	short getAmount() const { return amount; }
	short getMaxCapacity() const { return maxCapacity; }
	unsigned short getTicksToComplete() const { return ticksToComplete; }
	unsigned short getElapsedTicks() const { return elapsedTicks; }
	float getProgress() const { return ticksToComplete == 0 ? 1.f : ((float)elapsedTicks) / ticksToComplete; }
	void restore(unsigned short savedElapsedTicks);

private:
	QueueActionType type;
	unsigned char maxCapacity;

	unsigned short id;
	unsigned short amount;
	unsigned short levelId;

	unsigned short ticksToComplete;
	unsigned short elapsedTicks;

	unsigned short ticksToCompletePerInstance;
};
