#pragma once
enum class QueueActionType : char;
class QueueElement;

class AbstractQueueManager {
public:
	virtual ~AbstractQueueManager() = default;
	virtual void add(short number, QueueActionType type, short id, unsigned short localMaxCapacity) =0;
	virtual QueueElement* update() =0;
	virtual short getSize() const =0;
	virtual QueueElement* getAt(short i) =0;
};
