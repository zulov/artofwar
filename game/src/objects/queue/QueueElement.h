#pragma once

enum class QueueActionType : char;
enum class ActionType : char;

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
	float getProgress() const { return ((float)elapsedTicks) / ticksToComplete; }
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
