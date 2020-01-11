#pragma once
#include <vector>

enum class ActionType : char;
class QueueElement;

class QueueManager
{
public:
	explicit QueueManager(short maxCapacity);
	~QueueManager();

	void add(short value, ActionType type, short id, short localMaxCapacity);
	QueueElement* update(float time);
	short getSize() const;
	QueueElement* getAt(short i);
private:
	float getSecToComplete(ActionType type, short id, int level);
	float getSecPerInstance(ActionType type, short id, int level);

	std::vector<QueueElement*> queue;

	short maxCapacity;
};
