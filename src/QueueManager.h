#pragma once
#include "QueueElement.h"
#include <vector>

class QueueManager
{
public:
	QueueManager();
	~QueueManager();
	void add(short value, ObjectType type, short id);
	void remove(short value, ObjectType type, short id);
	std::vector<QueueElement*> * update(float time);
private:
	std::vector<QueueElement*> *queue;
	std::vector<QueueElement*>* ended;
};

