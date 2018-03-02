#pragma once
#include "QueueElement.h"
#include <vector>

class QueueManager
{
public:
	QueueManager(short _maxCapacity);
	~QueueManager();

	void add(short value, QueueType type, short id, short localMaxCapacity);
	QueueElement* update(float time);
	short getSize();
	QueueElement* getAt(short i);
private:
	float getSecToComplete(QueueType type, short id, int level);
	float getSecPerInstance(QueueType type, short id, int level);

	std::vector<QueueElement*> queue;

	short maxCapacity;
};
