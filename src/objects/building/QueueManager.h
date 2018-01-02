#pragma once
#include "QueueElement.h"
#include <vector>

class QueueManager
{
public:
	QueueManager(short _maxCapacity);
	~QueueManager();
	void add(short value, ObjectType type, short id);
	void remove(short value, ObjectType type, short id);
	QueueElement* update(float time);
	short getSize();
	QueueElement* getAt(short i);
private:
	std::vector<QueueElement*> queue;
	
	short maxCapacity;
};

