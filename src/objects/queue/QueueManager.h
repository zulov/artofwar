#pragma once
#include "QueueElement.h"
#include <vector>

class QueueManager
{
public:
	explicit QueueManager(short _maxCapacity);
	~QueueManager();

	void add(short value, ActionType type, short id, short localMaxCapacity);
	QueueElement* update(float time);
	short getSize();
	QueueElement* getAt(short i);
private:
	float getSecToComplete(ActionType type, short id, int level);
	float getSecPerInstance(ActionType type, short id, int level);

	std::vector<QueueElement*> queue;

	short maxCapacity;
};
