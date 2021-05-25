#pragma once
enum class QueueActionType : char;
class QueueElement;

class AbstractQueueManager {
public:
	virtual void add(short number, QueueActionType type, short id, unsigned short localMaxCapacity) =0;
	virtual QueueElement* update(float time) =0;
	virtual short getSize() const =0;
	virtual QueueElement* getAt(short i) =0;
};
